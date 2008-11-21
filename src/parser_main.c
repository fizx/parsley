#include <stdio.h>
#include "kstring.h"
#include "y.tab.h"

int main() {
  yyparse();
  return 0; 
}