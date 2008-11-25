#ifndef DEXTER_H_INCLUDED
#define DEXTER_H_INCLUDED

#define DEX_BUF_SIZE 1024

#include "obstack.h"

#define obstack_chunk_alloc malloc
#define obstack_chunk_free free

static struct obstack dex_obstack;
static char* dex_parsing_context;
static int dex_error_state = 0;
static int dex_debug_mode = 0;

#include <json/json.h>

char* dex_compile(char* dex, char* incl);
char* dex_set_debug_mode(int);
void dex_error(char*);

void* __dex_alloc(int);
void __dex_recurse(struct json_object *, struct printbuf*, char*);
void __dex_recurse_object(struct json_object *, struct printbuf*, char*);
void __dex_recurse_array(struct json_object *, struct printbuf*, char*);
void __dex_recurse_string(struct json_object *, struct printbuf*, char*);

#endif