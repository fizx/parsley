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
static int dex_debug_mode = 0;
static bool dex_exslt_registered = false;
static char* last_dex_error;

#include <json/json.h>

struct compiled_dex {
	xsltStylesheetPtr stylesheet;
	char* error;
};

typedef struct compiled_dex * dexPtr;

dexPtr dex_compile(char* dex, char* incl);
xmlDocPtr dex_parse_file(dexPtr, char*, boolean);
xmlDocPtr dex_parse_string(dexPtr, char*, size_t, boolean);

static xmlDocPtr dex_parse_doc(dexPtr, xmlDocPtr);

void* __dex_alloc(int);
static void __dex_recurse(struct json_object *, struct printbuf*, char*);
static void __dex_recurse_object(struct json_object *, struct printbuf*, char*);
static void __dex_recurse_array(struct json_object *, struct printbuf*, char*);
static void __dex_recurse_string(struct json_object *, struct printbuf*, char*);

#endif