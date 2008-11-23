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

int main () {
	char buffer[BUF_SIZE];
	while(fgets(buffer, BUF_SIZE, stdin)) {
		puts(myparse(buffer));
	}
	return 0;
}