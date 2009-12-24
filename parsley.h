#ifndef PARSLEY_H_INCLUDED
#define PARSLEY_H_INCLUDED

#define PARSLEY_BUF_SIZE 1024

#include <stdbool.h>
#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <json/json.h>
#include "parsed_xpath.h"


static int parsley_debug_mode = 0;
static char* last_parsley_error;


typedef struct __compiled_parsley {
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
	xmlNsPtr ns;
 	xmlNodePtr node;
	struct json_object * json;
	char* tag;
	pxpathPtr filter;
	pxpathPtr expr;
	bool magic;
	bool array;
	bool string;
  int flags;  //bitmask over following enum
	struct __parsley_context * parent;
	struct __parsley_context *child;
	struct __parsley_context *next;
} parsley_context;

enum {
  PARSLEY_OPTIONAL    = 1,
  PARSLEY_BANG        = 2
};

enum {
	PARSLEY_OPTIONS_HTML 					 = 1,
	PARSLEY_OPTIONS_PRUNE   			 = 2,
	PARSLEY_OPTIONS_ALLOW_NET  		 = 4,
	PARSLEY_OPTIONS_ALLOW_LOCAL    = 8,
	PARSLEY_OPTIONS_COLLATE        = 16,
	PARSLEY_OPTIONS_SGWRAP         = 32,
	PARSLEY_OPTIONS_FORCE_UTF8     = 64
};

typedef parsley_context * contextPtr;

void parsed_parsley_free(parsedParsleyPtr);

void killDefaultNS(xmlDocPtr doc);
void parsley_free(parsleyPtr);
parsleyPtr parsley_compile(char* parsley, char* incl);
parsedParsleyPtr parsley_parse_file(parsleyPtr parsley, char* file, int flags);
parsedParsleyPtr parsley_parse_string(parsleyPtr parsley, char* string, size_t size, char* base_uri, int flags);
parsedParsleyPtr parsley_parse_doc(parsleyPtr, xmlDocPtr, int);

void parsleyXsltError(void * ctx, const char * msg, ...);

void parsley_set_user_agent(char const *agent);
static contextPtr parsley_parsing_context;
	
#endif