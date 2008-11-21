#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/kstring.h"

struct foo_t {
  int a;
  int b;
};

int fail = 0;
#define FAIL(msg) (printf("FAIL: %s\n", msg), exit(1))

#define F struct foo_t
#define FP struct foo_t *

int main(){
  F foo;
  foo.a = 1;
  foo.b = 2;
  FP bar = (FP) memdup(&foo, sizeof(F));
  if(foo.a != bar->a) FAIL(1);
  // if(foo.b != bar->b) FAIL(2);
  if(&foo == bar) FAIL(2);
  
  char* a = "hello ";
  char* b = "world";
  
  if(strcmp(astrcat(a, b), "hello world") != 0) FAIL(3);
  
  
  char* c = "hel\nlo";
  
  if(strcmp(esc(c), "hel\\nlo") != 0) FAIL(astrcat3("escaping failed: '", c, "'"));
  
  return 0;
}