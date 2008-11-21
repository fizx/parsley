#include <stdio.h>
#include "kstring.h"
#include "y.tab.h"

int main() {
  const int max = 10000;
  char buffer[max];
  while (fgets(buffer, max, stdin) != NULL) {
    myparse(buffer);
  }
  return 0; 
}