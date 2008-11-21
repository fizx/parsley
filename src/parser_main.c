#include <stdio.h>
#include "kstring.h"
#include "y.tab.h"

int yywrap(void){
  return 1;
}

int main() {
  const int max = 1000;
  start_debugging();
  char buffer[max];
  while (fgets(buffer, max, stdin) != NULL) {
    myparse(buffer);
  }
  return 0; 
}