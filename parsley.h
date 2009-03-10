#ifndef PARSLEY_H_INCLUDED
#define PARSLEY_H_INCLUDED

#define PARSLEY_BUF_SIZE 1024

#include <stdbool.h>
#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>


static int parsley_debug_mode = 0;
static char* last_parsley_error;

#include <json/json.h>

typedef struct __compiled_parsley {
 	char* raw_stylesheet;
	xsltStylesheetPtr stylesheet;
	char* error;
} compiled_parsley;

typedef struct __parsed_parsley {
  xmlDocPtr xml;
	char *error;
  compiled_parsley *parsley;
} parsed_parsley;

typedef compiled_parsley * parsleyPtr;
typedef parsed_parsley * parsedParsleyPtr;

typedef struct __key_node {
	char* name;
	char* use;
	struct __key_node * next;
} key_node;

typedef key_node * keyPtr;

typedef struct __parsley_context {
	struct printbuf * buf;
	struct printbuf * key_buf;
 	keyPtr keys;
	struct json_object * json;
	struct __parsley_context * parent;
	char* tag;
	char* filter;
	char* expr;
	char* raw_expr;
	char* full_expr;
	char* name;
	bool magic;
	bool array;
	bool string;
  int flags;  //bitmask over following enum
} parsley_context;

enum {
   PARSLEY_OPTIONAL    = 1,
};

typedef parsley_context * contextPtr;

void parsed_parsley_free(parsedParsleyPtr);

void parsley_free(parsleyPtr);
parsleyPtr parsley_compile(char* parsley, char* incl);
parsedParsleyPtr parsley_parse_file(parsleyPtr, char*, bool);
parsedParsleyPtr parsley_parse_string(parsleyPtr, char*, size_t, bool);
parsedParsleyPtr parsley_parse_doc(parsleyPtr, xmlDocPtr);


static contextPtr parsley_parsing_context;

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

static void __parsley_recurse(contextPtr);
static char* filter_intersection(char*, char*);

static char* inner_key_of(struct json_object *);
static char* inner_key_each(struct json_object *);

static void visit(parsedParsleyPtr ptr, xmlNodePtr xml, char* err);
static bool xml_empty(xmlNodePtr xml);
	
#endif