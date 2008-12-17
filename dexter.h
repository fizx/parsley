#ifndef DEXTER_H_INCLUDED
#define DEXTER_H_INCLUDED

#define DEX_BUF_SIZE 1024

#include "obstack.h"
#include <stdbool.h>
#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>

#define obstack_chunk_alloc malloc
#define obstack_chunk_free free

static struct obstack dex_obstack;
static char* dex_parsing_context;
// static int dex_error_state = 0;
static int dex_debug_mode = 0;
static bool dex_exslt_registered = false;
static char* last_dex_error;

#include <json/json.h>

struct compiled_dex {
	xsltStylesheetPtr stylesheet;
	int flags;
	char* error;
};

typedef struct compiled_dex * dexPtr;

dexPtr dex_compile(char* dex);
dexPtr dex_compile_with(char* dex, char* incl, int flags);
char* dex_parse(dexPtr);
// 
// char* dex_last_error(void);
// static void dex_compilation_error(char*);

void* __dex_alloc(int);
static void __dex_recurse(struct json_object *, struct printbuf*, char*);
static void __dex_recurse_object(struct json_object *, struct printbuf*, char*);
static void __dex_recurse_array(struct json_object *, struct printbuf*, char*);
static void __dex_recurse_string(struct json_object *, struct printbuf*, char*);

#endif