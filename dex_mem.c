#include "dex_mem.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

static struct obstack dex_obstack;
static bool dex_obstack_initialized = false;

void dex_collect() {
	obstack_free(&dex_obstack, NULL);
	obstack_init(&dex_obstack);
}

void * dex_alloc(int size) {
  if(!dex_obstack_initialized) {
		obstack_init(&dex_obstack);
		dex_obstack_initialized = true;
	}
  void * mem = obstack_alloc(&dex_obstack, size);
  void * ptr = mem;
  for(int i = 0; i < size; i++)
  {
    *(char *)(mem + i) = '\0';
  }
  return mem;
}
