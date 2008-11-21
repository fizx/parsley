#include <stdio.h>
#include <stdlib.h>
#include "y.tab.h"

int yywrap(void){
  exit(0);
}

int main(void){
	while(1) yylex();
  return 0;
}