#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include <string.h>
#include <json/printbuf.h>
#include "parsley.h"
#include "util.h"

struct list_elem {
	int has_next;
	struct list_elem *next;
	char *string;
};

struct arguments
{
	struct list_elem *include_files;
	char *parsley;
  char *output_file;
};

const char *argp_program_version = "parsleyc 0.1";
const char *argp_program_bug_address = "<kyle@kylemaxwell.com>";
static char args_doc[] = "DEX_FILE";
static char doc[] = "Parsleyc is a parselet to XSLT compiler";

static struct argp_option options[] = {
  {"debug",    'd', 0, 0, 	"Turn on Bison parser debugging" },
	{"output",   'o', "FILE", 0, 	"Output to FILE instead of standard output" },
  {"include",  'i', "FILE", 0, 	"Include the contents of FILE in the produced XSLT" },
  { 0 }
};

static error_t parse_opt (int key, char *arg, struct argp_state *state)
{
  struct arguments *arguments = state->input;
	struct list_elem *base = arguments->include_files;
	struct list_elem *e;

  switch (key)
    {
    case 'i':
			e = (struct list_elem *) calloc(1, sizeof(e));
			e->string = arg;
			while(base->has_next) base = base->next;
			base->next = e;
			base->has_next = 1;
      break;
    case 'd':	
			// parsley_set_debug_mode(1);
			break;
    case 'o':
      arguments->output_file = arg;
      break;
    case ARGP_KEY_ARG:
      if (state->arg_num >= 1) argp_usage (state);
      arguments->parsley = arg;
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
	struct arguments arguments;
	struct list_elem elem;
	struct list_elem *elemptr = &elem;
	elem.has_next = 0;
	
	arguments.include_files = elemptr;
	arguments.output_file = "-";
	arguments.parsley = "-";
	argp_parse (&argp, argc, argv, 0, 0, &arguments);
	
	struct printbuf* parsley = printbuf_new();
	struct printbuf* incl = printbuf_new();
  sprintbuf(parsley, "");
  sprintbuf(incl, "");

	FILE* in = parsley_fopen(arguments.parsley, "r");
	
	printbuf_file_read(in, parsley);
	while(elemptr->has_next) {
		elemptr = elemptr->next;
		FILE* f = parsley_fopen(elemptr->string, "r");
		printbuf_file_read(f, incl);
		fclose(f);
	}
	
	parsleyPtr compiled = parsley_compile(parsley->buf, incl->buf);
	if(compiled->error != NULL) {
		fprintf(stderr, "%s\n", compiled->error);
	 	exit(1);
	}
	
  FILE* fo = parsley_fopen(arguments.output_file, "w");
  xmlDocFormatDump(fo, compiled->stylesheet->doc, 1);
  fclose(fo);
  
	return 0;
}

