#include "dex_mem.h"
#include <stdlib.h>

struct obstack * dex_obstack = NULL;

void dex_collect() {
	obstack_free(&dex_obstack, NULL);
  dex_obstack = NULL;
}

void * dex_alloc(int size) {
  if(dex_obstack == NULL) obstack_init(&dex_obstack);
	return obstack_alloc(&dex_obstack, size);
}
