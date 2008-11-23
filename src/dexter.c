#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include <json/json.h>
#include "kstring.h"
#include "obstack.h"
#include "y.tab.h"
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <stdbool.h>
#define BUF_SIZE 1024

int yywrap(void){
  return 1;
}

int tracing = 0;
char* parsing_context;

const char *argp_program_version = "dexter 0.1";
const char *argp_program_bug_address = "<kyle@kylemaxwell.com>";
static char args_doc[] = "DEX_FILE";
static char doc[] = "Dexter is a dex to XSLT compiler";

static struct argp_option options[] = {
	{"debug",    'd', 0, 0, 	"Turn on Bison parser debugging" },
	{"output",   'o', "FILE", 0, 	"Output to FILE instead of standard output" },
  {"include",  'i', "FILE", 0, 	"Include the contents of FILE in the produced XSLT" },
  { 0 }
};

struct list_elem {
	int has_next;
	struct list_elem *next;
	char *string;
};

typedef struct arguments
{
	struct list_elem *include_files;
	char *dex;
  char *output_file;
}	ARGS;

int compile(ARGS*);
void recurse(struct json_object *, FILE *, char*);
void recurse_object(struct json_object *, FILE *, char*);
void recurse_array(struct json_object *, FILE *, char*);
void recurse_string(struct json_object *, FILE *, char*);

static error_t parse_opt (int key, char *arg, struct argp_state *state)
{
  struct arguments *arguments = state->input;
	struct list_elem *base = arguments->include_files;
	struct list_elem *e;

  switch (key)
    {
    case 'i':
			e = (struct list_elem *) malloc(sizeof(e));
			e->string = arg;
			while(base->has_next) base = base->next;
			base->next = e;
			base->has_next = 1;
      break;
    case 'd':	
			start_debugging();
			tracing = 1; 
			break;
    case 'o':
      arguments->output_file = arg;
      break;
    case ARGP_KEY_ARG:
      if (state->arg_num >= 1) argp_usage (state);
      arguments->dex = arg;
      break;
    case ARGP_KEY_END:
      if (state->arg_num < 1) argp_usage (state);
      break;
    default:
      return ARGP_ERR_UNKNOWN;
    }
  return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc };

int main (int argc, char **argv) {
	printbuf_new();
	ARGS arguments;
	struct list_elem include_root;
	struct list_elem *f;
	arguments.include_files = &include_root;
	arguments.output_file = "-";
	arguments.dex = "-";
	argp_parse (&argp, argc, argv, 0, 0, &arguments);
	return compile(&arguments);
}

int compile(ARGS* arguments) {
	struct json_object *json = json_object_from_file(arguments->dex);
	if(is_error(json)) {
		fprintf(stderr, "Your dex is not valid json.\n");
		exit(1);
	}
	
	FILE* out = (strcmp(arguments->output_file, "-") == 0) ? stdout : fopen(arguments->output_file, "w");
	fprintf(out, "<xsl:stylesheet version=\"1.0\" xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\"");
	fprintf(out, " xmlns:str=\"http://exslt.org/strings\"");
	fprintf(out, " xmlns:set=\"http://exslt.org/sets\"");
	fprintf(out, " xmlns:math=\"http://exslt.org/math\"");
	fprintf(out, " xmlns:func=\"http://exslt.org/functions\"");
	fprintf(out, " xmlns:user=\"http://kylemaxwell.com/dexter/user-functions\"");
	fprintf(out, " xmlns:dyn=\"http://exslt.org/dynamic\"");
	fprintf(out, " xmlns:date=\"http://exslt.org/dates-and-times\"");
	fprintf(out, " xmlns:exsl=\"http://exslt.org/common\"");
	fprintf(out, " xmlns:saxon=\"http://icl.com/saxon\"");
	fprintf(out, " exclude-result-prefixes=\"str math set func dyn exsl saxon user date\"");
	fprintf(out, ">\n");
	fprintf(out, "<xsl:output method=\"xml\" indent=\"yes\"/>\n");
	fprintf(out, "<xsl:strip-space elements=\"*\"/>\n");
	
	struct list_elem *elem = arguments->include_files;
	while(elem->has_next) {
		elem = elem->next;
		FILE* incl = fopen(elem->string, "r");
		if (incl == NULL){  
		    fprintf(stderr, "Cannot open file %s, error %d, %s\n", elem->string, errno, strerror(errno));
				exit(1);
		}
		char c;
		while((c = fgetc(incl)) != EOF) fputc(c, out);
		fclose(incl);
	}
	
	fprintf(out, "<xsl:template match=\"/\">\n");
	fprintf(out, "<root>\n");
	
	char *context = "root";
	recurse(json, out, context);
	
	fprintf(out, "</root>\n");
	fprintf(out, "</xsl:template>\n");
	fprintf(out, "</xsl:stylesheet>\n");
	json_object_put(json);
	fclose(out);
	return 0;
}

void recurse_object(struct json_object * json, FILE * out, char *context) {
	char *tag;
	char *ptr;
	char *expr;
	int offset;
	bool has_expr;
	json_object_object_foreach(json, key, val) {
		offset = 0;
		tag = astrdup(key);
		expr = astrdup(key);
		ptr = tag;
		has_expr = false;
		while(*ptr++ != '\0'){
			offset++;
			if(*ptr == '(') {
				*ptr = 0;
				has_expr = true;
				break;
			}
		}
		expr += offset;
		
		fprintf(out, "<%s>\n", tag);
		if(has_expr) fprintf(out, "<xsl:for-each select=\"%s\">\n", myparse(expr));
		recurse(val, out, astrcat3(context, ".", tag));
		if(has_expr) fprintf(out, "</xsl:for-each>\n");
		fprintf(out, "</%s>\n", tag);
  }
}

void recurse_array(struct json_object * json, FILE * out, char *context) {
	for(int i = 0; i < json_object_array_length(json); i++) {
		fprintf(out, "<group>\n");
 		recurse(json_object_array_get_idx(json, i), out, context);
    fprintf(out, "</group>\n");
  }
}

void recurse_string(struct json_object * json, FILE * out, char *context) {
	char* a = astrdup(json_object_get_string(json));
	char* ptr = context;
	char* last = context;
	char* expr;
	while(*ptr++){
		if(*ptr == '.') last = ptr + 1;
	}
	parsing_context = context;
	expr = myparse(a);
	fprintf(out, "<xsl:variable name=\"%s\" select=\"%s\" />\n", context, expr);
	fprintf(out, "<xsl:variable name=\"%s\" select=\"$%s\" />\n", last, context);
	fprintf(out, "<xsl:value-of select=\"$%s\" />\n", context);
}

void yyerror (const char * s) {
  printf("%s in key: %s\n", s, parsing_context);
  exit(1);
}


void recurse(struct json_object * json, FILE * out, char *context) {
	switch(json_object_get_type(json)){
		case json_type_object:
			recurse_object(json, out, context);
			break;
		case json_type_array:
			recurse_array(json, out, context);
			break;
		case json_type_string:
			recurse_string(json, out, context);
			break;
		case json_type_boolean:
		case json_type_double:
		case json_type_int:
		default:
			fprintf(stderr, "Invalid type in json\n");
			exit(1);
	}
}
