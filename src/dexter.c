#include <stdio.h>
#include <argp.h>
#include <json/json.h>
#include "kstring.h"
#include "y.tab.h"


int yywrap(void){
  return 1;
}

const char *argp_program_version = "dexter 0.1";
const char *argp_program_bug_address = "<kyle@kylemaxwell.com>";
static char args_doc[] = "DEX_FILE";
static char doc[] = "Dexter is a dex to XSLT compiler";

static struct argp_option options[] = {
	{"output",   'o', "FILE", 0, 	"Output to FILE instead of standard output" },
  {"include",  'i', "FILE", 0, 	"Include the contents of FILE in the produced XSLT" },
  { 0 }
};

struct list_elem {
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
			while(base->next != NULL) base = base->next;
			base->next = e;
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
	return 0;
}
