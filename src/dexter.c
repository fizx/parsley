#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include <json/json.h>
#include "kstring.h"
#include "obstack.h"
#include "y.tab.h"
#include "printbuf.h"
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
void recurse(struct json_object *, struct printbuf*, char*);
void recurse_object(struct json_object *, struct printbuf*, char*);
void recurse_array(struct json_object *, struct printbuf*, char*);
void recurse_string(struct json_object *, struct printbuf*, char*);

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
	struct printbuf* buf = printbuf_new();
	if(is_error(json)) {
		fprintf(stderr, "Your dex is not valid json.\n");
		exit(1);
	}
	
	sprintbuf(buf, "<xsl:stylesheet version=\"1.0\" xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\"");
	sprintbuf(buf, " xmlns:str=\"http://exslt.org/strings\"");
	sprintbuf(buf, " xmlns:set=\"http://exslt.org/sets\"");
	sprintbuf(buf, " xmlns:math=\"http://exslt.org/math\"");
	sprintbuf(buf, " xmlns:func=\"http://exslt.org/functions\"");
	sprintbuf(buf, " xmlns:user=\"http://kylemaxwell.com/dexter/user-functions\"");
	sprintbuf(buf, " xmlns:dyn=\"http://exslt.org/dynamic\"");
	sprintbuf(buf, " xmlns:date=\"http://exslt.org/dates-and-times\"");
	sprintbuf(buf, " xmlns:exsl=\"http://exslt.org/common\"");
	sprintbuf(buf, " xmlns:saxon=\"http://icl.com/saxon\"");
	sprintbuf(buf, " exclude-result-prefixes=\"str math set func dyn exsl saxon user date\"");
	sprintbuf(buf, ">\n");
	sprintbuf(buf, "<xsl:output method=\"xml\" indent=\"yes\"/>\n");
	sprintbuf(buf, "<xsl:strip-space elements=\"*\"/>\n");
	
	struct list_elem *elem = arguments->include_files;
	char *t = (char*) malloc(sizeof(char) * BUF_SIZE);
	while(elem->has_next) {
		elem = elem->next;
		FILE* incl = fopen(elem->string, "r");
		if (incl == NULL){  
		    fprintf(stderr, "Cannot open file %s, error %d, %s\n", elem->string, errno, strerror(errno));
				exit(1);
		}
		
		while((t = fgets(t, BUF_SIZE, incl)) != NULL) printbuf_memappend(buf, t, strlen(t));
		fclose(incl);
	}
	free(t);
	
	sprintbuf(buf, "<xsl:template match=\"/\">\n");
	sprintbuf(buf, "<root>\n");
	
	char *context = "root";
	recurse(json, buf, context);
	
	sprintbuf(buf, "</root>\n");
	sprintbuf(buf, "</xsl:template>\n");
	sprintbuf(buf, "</xsl:stylesheet>\n");
	json_object_put(json);
	FILE* out = (strcmp(arguments->output_file, "-") == 0) ? stdout : fopen(arguments->output_file, "w");
	fprintf(out, buf->buf);
	printbuf_free(buf);
	fclose(out);
	return 0;
}

void recurse_object(struct json_object * json, struct printbuf* buf, char *context) {
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
		
		sprintbuf(buf, "<%s>\n", tag);
		if(has_expr) sprintbuf(buf, "<xsl:for-each select=\"%s\">\n", myparse(expr));
		recurse(val, buf, astrcat3(context, ".", tag));
		if(has_expr) sprintbuf(buf, "</xsl:for-each>\n");
		sprintbuf(buf, "</%s>\n", tag);
  }
}

void recurse_array(struct json_object * json, struct printbuf* buf, char *context) {
	for(int i = 0; i < json_object_array_length(json); i++) {
		sprintbuf(buf, "<group>\n");
 		recurse(json_object_array_get_idx(json, i), buf, context);
    sprintbuf(buf, "</group>\n");
  }
}

void recurse_string(struct json_object * json, struct printbuf* buf, char *context) {
	char* a = astrdup(json_object_get_string(json));
	char* ptr = context;
	char* last = context;
	char* expr;
	while(*ptr++){
		if(*ptr == '.') last = ptr + 1;
	}
	parsing_context = context;
	expr = myparse(a);
	sprintbuf(buf, "<xsl:variable name=\"%s\" select=\"%s\" />\n", context, expr);
	sprintbuf(buf, "<xsl:variable name=\"%s\" select=\"$%s\" />\n", last, context);
	sprintbuf(buf, "<xsl:value-of select=\"$%s\" />\n", context);
}

void yyerror (const char * s) {
  printf("%s in key: %s\n", s, parsing_context);
  exit(1);
}


void recurse(struct json_object * json, struct printbuf* buf, char *context) {
	switch(json_object_get_type(json)){
		case json_type_object:
			recurse_object(json, buf, context);
			break;
		case json_type_array:
			recurse_array(json, buf, context);
			break;
		case json_type_string:
			recurse_string(json, buf, context);
			break;
		case json_type_boolean:
		case json_type_double:
		case json_type_int:
		default:
			fprintf(stderr, "Invalid type in json\n");
			exit(1);
	}
}
