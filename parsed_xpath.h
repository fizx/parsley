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
   PXPATH_FUNCTION,
	 PXPATH_PATH,
	 PXPATH_LITERAL
};

pxpathPtr pxpath_new(int type, char* value);
pxpathPtr pxpath_new_func(char* value, pxpathPtr child);
pxpathPtr pxpath_cat_paths(int n, ...);
pxpathPtr pxpath_new_path(int n, ...);
pxpathPtr pxpath_new_literal(int n, ...);
void pxpath_free(pxpathPtr ptr);
char* pxpath_to_string(pxpathPtr ptr);

#endif
