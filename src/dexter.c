#include <stdio.h>
#include <argp.h>
#include "kstring.h"
#include "y.tab.h"

int yywrap(void){
  return 1;
}
const char *argp_program_version = "dexter 0.1";
const char *argp_program_bug_address = "<kyle@kylemaxwell.com>";

int main (int argc, char **argv) {
  argp_parse(0, argc, argv, 0, 0, 0);
  return 0; 
}