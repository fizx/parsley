#ifndef DEXTER_H_INCLUDED
#define DEXTER_H_INCLUDED

#define DEX_BUF_SIZE 1024

#include <stdbool.h>
#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>


static int dex_debug_mode = 0;
static char* last_dex_error;

#include <json/json.h>

typedef struct __compiled_dex {
 	char* raw_stylesheet;
	xsltStylesheetPtr stylesheet;
	char* error;
} compiled_dex;

typedef struct __parsed_dex {
  xmlDocPtr xml;
	char *error;
  compiled_dex *dex;
} parsed_dex;

typedef compiled_dex * dexPtr;
typedef parsed_dex * parsedDexPtr;

typedef struct __key_node {
	char* name;
	char* use;
	struct __key_node * next;
} key_node;

typedef key_node * keyPtr;

typedef struct __dex_context {
	struct printbuf * buf;
	struct printbuf * key_buf;
 	keyPtr keys;
	struct json_object * json;
	struct __dex_context * parent;
	char* tag;
	char* filter;
	char* expr;
	char* raw_expr;
	char* full_expr;
	char* name;
	char* magic;
	int array;
	int string;
  int flags;
} dex_context;

typedef dex_context * contextPtr;

void parsed_dex_free(parsedDexPtr);

void dex_free(dexPtr);
dexPtr dex_compile(char* dex, char* incl);
xmlDocPtr dex_parse_file(dexPtr, char*, boolean);
xmlDocPtr dex_parse_string(dexPtr, char*, size_t, boolean);
xmlDocPtr dex_parse_doc(dexPtr, xmlDocPtr);

enum {
   DEX_OPTIONAL    = 1,
};

static contextPtr dex_parsing_context;

static char* full_expr(contextPtr, char*);
static char* expr_join(char*, char*);
static char* inner_key_of(struct json_object *);
static char* inner_key_each(struct json_object *);
static void free_context(contextPtr);
static contextPtr init_context();
static contextPtr clone_context(contextPtr);
static contextPtr tagged_context(contextPtr, char*);

static contextPtr new_context(struct json_object *, struct printbuf *);
static contextPtr deeper_context(contextPtr, char*, struct json_object *);

static void __dex_recurse(contextPtr);
static char* filter_intersection(char*, char*);

static char* inner_key_of(struct json_object *);
static char* inner_key_each(struct json_object *);

static void visit(dexPtr dex, xmlNodePtr xml, bool bubbling);
	
#endif