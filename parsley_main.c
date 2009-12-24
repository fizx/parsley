#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json/printbuf.h>
#include "parsley.h"
#include "xml2json.h"
#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxml/parser.h>
#include <libxml/HTMLparser.h>
#include <libxml/HTMLtree.h>
#include <libxml/xmlwriter.h>
#include <json/json.h>
#include <argp.h>
#include "util.h"

struct arguments
{
	struct list_elem *include_files;
	int flags;
	int output_xml;
	char *parsley;
	char *user_agent;
  char *input_file;
  char *output_file;
};

struct list_elem {
	int has_next;
	struct list_elem *next;
	char *string;
};

const char *argp_program_version = "parsley 0.1";
const char *argp_program_bug_address = "<kyle@kylemaxwell.com>";
static char args_doc[] = "PARSELET FILE_TO_PARSE";
static char doc[] = "Parsley is a parselet parser.";

static struct argp_option options[] = {
	{"input-xml",       'x', 0, 0, 	"Use the XML parser (not HTML)" },
	{"output-xml",      'X', 0, 0, 	"Output XML (not JSON)" },
	{"output",   				'o', "FILE", 0, 	"Output to FILE instead of standard output" },
  {"include",  				'i', "FILE", 0, 	"Include the contents of FILE in the compiled XSLT" },
	{"no-prune",        'n', 0, 0, 	"Don't prune empty subtrees" },
	{"no-collate",      'N', 0, 0, 	"Don't collate array entries" },
	{"sg-wrap",         's', 0, 0, 	"Wrap text nodes for SelectorGadget compatibility" },
	{"user-agent",      'U', "USER_AGENT", 0, 	"Value of HTTP User-Agent header" },
	{"utf8",            'u', 0, 0, 	"Force input to be read as UTF-8" },
	{"no-net",          'z', 0, 0, 	"Disable ftp and http access for parselets" },
	{"no-filesystem",   'Z', 0, 0, 	"Disable filesystem access for parselets" },
  { 0 }
};

static error_t parse_opt (int key, char *arg, struct argp_state *state)
{
  struct arguments *arguments = state->input;
	struct list_elem *base = arguments->include_files;
	struct list_elem *e;

  switch (key)
    {	
    case 'x':
      arguments->flags &= ~PARSLEY_OPTIONS_HTML;
			break;
    case 'u':
      arguments->flags |= PARSLEY_OPTIONS_FORCE_UTF8;
			break;
		case 'U':
      parsley_set_user_agent(arg);
    case 'n':
			arguments->flags &= ~PARSLEY_OPTIONS_PRUNE;
			break;
    case 'N':
			arguments->flags &= ~PARSLEY_OPTIONS_COLLATE;
			break;
    case 'z':
			arguments->flags &= ~PARSLEY_OPTIONS_ALLOW_NET;
			break;
    case 's':
			arguments->flags |= PARSLEY_OPTIONS_SGWRAP;
			break;
    case 'Z':
			arguments->flags &= ~PARSLEY_OPTIONS_ALLOW_LOCAL;
			break;
    case 'X':
			arguments->output_xml = 1;
			break;
    case 'i':
			e = (struct list_elem *) calloc(1, sizeof(e));
			e->string = arg;
			while(base->has_next) base = base->next;
			base->next = e;
			base->has_next = 1;
      break;
    case 'o':
      arguments->output_file = arg;
      break;
    case ARGP_KEY_ARG:
			switch(state->arg_num){
			case 0:
				arguments->parsley = arg;
				break;
			case 1:
				arguments->input_file = arg;
				break;
			default:
				argp_usage (state);
			}
      break;
    case ARGP_KEY_END:
      if (state->arg_num < 2) argp_usage (state);
      break;
    default:
      return ARGP_ERR_UNKNOWN;
    }
  return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc };

int main (int argc, char **argv) {
	struct arguments arguments;
	struct list_elem elem;
	struct list_elem *elemptr = &elem;
	elem.has_next = 0;
	arguments.output_xml = 0;
  arguments.flags = ~0 & ~PARSLEY_OPTIONS_SGWRAP & ~PARSLEY_OPTIONS_FORCE_UTF8;
	arguments.include_files = elemptr;
	arguments.output_file = "-";
	argp_parse (&argp, argc, argv, 0, 0, &arguments);
	
 	struct printbuf *buf = printbuf_new();
 	struct printbuf *incl = printbuf_new();
  sprintbuf(buf, "");
  sprintbuf(incl, "");
  
  FILE * fd = parsley_fopen(arguments.parsley, "r");
  printbuf_file_read(fd, buf);
  fclose(fd);

	while(elemptr->has_next) {
		elemptr = elemptr->next;
		FILE* f = parsley_fopen(elemptr->string, "r");
		printbuf_file_read(f, incl);
		fclose(f);
	}
	
  // printf("a\n"); 
	parsleyPtr compiled = parsley_compile(buf->buf, incl->buf);
  // printf("b\n");

	if(compiled->error != NULL) {
		fprintf(stderr, "%s\n", compiled->error);
		exit(1);
	}
	
	parsedParsleyPtr ptr = parsley_parse_file(compiled, arguments.input_file, arguments.flags);

	if(ptr->error != NULL) {
		fprintf(stderr, "Parsing failed: %s\n", ptr->error);
		exit(1);
	}
	
	if(arguments.output_xml) {
		xmlSaveFormatFile(arguments.output_file, ptr->xml, 1);	
	} else {
	  struct json_object *json = xml2json(ptr->xml->children->children);
    if(json == NULL) {
      fprintf(stderr, "xml2json unknown error");
      exit(1);
    }
    char * json_string = json_object_to_json_string(json);
		FILE* f = parsley_fopen(arguments.output_file, "w");
	  fprintf(f, "%s\n", json_string);
    json_object_put(json);
		fclose(f);
	}
	
  printbuf_free(buf);
  printbuf_free(incl);
	parsley_free(compiled);
	parsed_parsley_free(ptr);
	return 0;
}