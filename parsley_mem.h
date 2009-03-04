#ifndef PARSLEY_MEM_H_INCLUDED
#define PARSLEY_MEM_H_INCLUDED

#define obstack_chunk_alloc malloc
#define obstack_chunk_free free

#include "obstack.h"

void parsley_collect();
void * parsley_alloc(int size);


#endif