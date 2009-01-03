#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "kstring.h"
#include "printbuf.h"
#include "dexter.h"
#include "xml2json.h"
#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxml/parser.h>
#include <libxml/HTMLparser.h>
#include <libxml/HTMLtree.h>
#include <libxml/xmlwriter.h>
#include "json-c-0.7/json.h"
#include <argp.h>
#include "util.h"

void dex_error(char* msg){
	fprintf(stderr, "%s\n", msg);
  exit(1);
}

struct arguments
{
	struct list_elem *include_files;
	int input_xml;
	int output_xml;
	char *dex;
  char *input_file;
  char *output_file;
};

struct list_elem {
	int has_next;
	struct list_elem *next;
	char *string;
};

const char *argp_program_version = "dexter 0.1";
const char *argp_program_bug_address = "<kyle@kylemaxwell.com>";
static char args_doc[] = "DEX_FILE FILE_TO_PARSE";
static char doc[] = "Dexter is a dex parser.";

static struct argp_option options[] = {
	{"input-xml",       'x', 0, 0, 	"Use the XML parser (not HTML)" },
	{"output-xml",      'X', 0, 0, 	"Output XML (not JSON)" },
	{"output",   				'o', "FILE", 0, 	"Output to FILE instead of standard output" },
  {"include",  				'i', "FILE", 0, 	"Include the contents of FILE in the compiled XSLT" },
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
			arguments->input_xml = 1;
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
				arguments->dex = arg;
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
	arguments.input_xml = 0;
	arguments.output_xml = 0;
	arguments.include_files = elemptr;
	arguments.output_file = "-";
	argp_parse (&argp, argc, argv, 0, 0, &arguments);

 	struct printbuf *buf = printbuf_new();
 	struct printbuf *incl = printbuf_new();
  
  FILE * fd = dex_fopen(arguments.dex, "r");
  printbuf_file_read(fd, buf);

	while(elemptr->has_next) {
		elemptr = elemptr->next;
		FILE* f = dex_fopen(elemptr->string, "r");
		printbuf_file_read(f, incl);
		fclose(f);
	}
		
	dexPtr compiled = dex_compile(buf->buf, incl->buf);
	
	xmlDocPtr xml = dex_parse_file(compiled, arguments.input_file, !(arguments.input_xml));
	if(xml == NULL) {
		fprintf(stderr, "Parsing failed: %s\n", compiled->error);
		exit(1);
	}
	
	if(arguments.output_xml) {
		xmlSaveFormatFile(arguments.output_file, xml, 1);	
	} else {
	  struct json_object *json = xml2json(xml->children->children);
		FILE* f = dex_fopen(arguments.output_file, "w");
	  fprintf(f, "%s\n", json_object_to_json_string(json));
		fclose(f);
	}
	
	return 0;
}