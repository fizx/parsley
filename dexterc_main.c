#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include <string.h>
#include "kstring.h"
#include "printbuf.h"
#include "dexter.h"

struct list_elem {
	int has_next;
	struct list_elem *next;
	char *string;
};

struct arguments
{
	struct list_elem *include_files;
	char *dex;
  char *output_file;
};

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
			// dex_set_debug_mode(1);
			break;
    case 'o':
      arguments->output_file = arg;
      break;
    case ARGP_KEY_ARG:
      if (state->arg_num >= 1) argp_usage (state);
      arguments->dex = arg;
      break;
    case ARGP_KEY_END:
      break;
    default:
      return ARGP_ERR_UNKNOWN;
    }
  return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc };

void dex_error(char* msg){
	fprintf(stderr, "%s\n", msg);
}

int main (int argc, char **argv) {
	struct arguments arguments;
	struct list_elem elem;
	struct list_elem *elemptr = &elem;
	elem.has_next = 0;
	
	arguments.include_files = elemptr;
	arguments.output_file = "-";
	arguments.dex = "-";
	argp_parse (&argp, argc, argv, 0, 0, &arguments);
	
	struct printbuf* dex = printbuf_new();
	struct printbuf* incl = printbuf_new();

	FILE* in = (strcmp(arguments.dex, "-") == 0) ? stdout : fopen(arguments.dex, "r");
	if(in == NULL) {		
    fprintf(stderr, "Cannot open file %s, error %d, %s\n", arguments.dex, errno, strerror(errno));
		exit(1);
	}
	
	printbuf_file_read(in, dex);
	while(elemptr->has_next) {
		elemptr = elemptr->next;
		FILE* f = fopen(elemptr->string, "r");
		if(f == NULL) {		
	    fprintf(stderr, "Cannot open file %s, error %d, %s\n", elemptr->string, errno, strerror(errno));
			exit(1);
		}
		printbuf_file_read(f, incl);
		fclose(f);
	}
	
	dexPtr compiled = dex_compile(dex->buf, incl->buf);
	if(compiled->error != NULL) {
		fprintf(stderr, "%s\n", compiled->error);
	 	exit(1);
	}
	
	FILE* fo;
	if(!strcmp("-", arguments.output_file)) {
		fo = stdout;
	} else {
		fo = fopen(arguments.output_file, "w");
	}
	fprintf(fo, compiled->raw_stylesheet);
	
	return 0;
}

