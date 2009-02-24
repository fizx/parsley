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
	return obstack_alloc(&dex_obstack, size);
}
