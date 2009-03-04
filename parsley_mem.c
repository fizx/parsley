#include "parsley_mem.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

static struct obstack parsley_obstack;
static bool parsley_obstack_initialized = false;

void parsley_collect() {
	obstack_free(&parsley_obstack, NULL);
	obstack_init(&parsley_obstack);
}

void * parsley_alloc(int size) {
  if(!parsley_obstack_initialized) {
		obstack_init(&parsley_obstack);
		parsley_obstack_initialized = true;
	}
  void * mem = obstack_alloc(&parsley_obstack, size);
  void * ptr = mem;
  for(int i = 0; i < size; i++)
  {
    *(char *)(mem + i) = '\0';
  }
  return mem;
}
