#ifndef _parsed_xpath_h_
#define _parsed_xpath_h_

#include <stdarg.h>

typedef struct __pxpath_node {
	int type;
	char* value;
 	struct __pxpath_node * next;
 	struct __pxpath_node * child;
} pxpath_node;

typedef pxpath_node * pxpathPtr;

enum {
   PXPATH_FUNCTION = 1,
	 PXPATH_PATH = 2,
	 PXPATH_LITERAL = 3,
	 PXPATH_LIT_EXPR = 4,
	 PXPATH_OPERATOR = 3
};

pxpathPtr pxpath_new(int type, char* value);
pxpathPtr pxpath_new_func(char* value, pxpathPtr child);
pxpathPtr pxpath_cat_paths(int n, ...);
pxpathPtr pxpath_cat_literals(int n, ...);
pxpathPtr pxpath_new_path(int n, ...);
pxpathPtr pxpath_dup(pxpathPtr p);
pxpathPtr pxpath_new_literal(int n, ...);
pxpathPtr pxpath_new_operator(int n, ...);
void pxpath_free(pxpathPtr ptr);
char* pxpath_to_string(pxpathPtr ptr);

#endif
