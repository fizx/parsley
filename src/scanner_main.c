#include <stdio.h>
#include <stdlib.h>
#include "y.tab.h"

int yywrap(void){
  exit(0);
}

int main(void){
  yylex();
  return 0;
}