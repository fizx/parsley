// typedef struct __pxpath_node {
// 	int type;
// 	char* value;
//  	__pxpath_node * next;
//  	__pxpath_node * child;
// } pxpath_node;
// 
// typedef pxpath_node pxpathPtr;
// 
// enum {
//    PXPATH_FUNCTION,
// 	 PXPATH_PATH
// };

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "parsed_xpath.h"
#include <json/printbuf.h>

pxpathPtr pxpath_new(int type, char* value) {
	pxpathPtr ptr = (pxpathPtr) calloc(sizeof(pxpath_node), 1);
	ptr->value = strdup(value);
	ptr->type = type;
	ptr->next = NULL;
	ptr->child = NULL;
	return ptr;
}

// pxpathPtr pxpath_cat_paths(int n, va_list) {
// 	
// }
// pxpathPtr pxpath_new_paths(int n, va_list) {
// 	
// }

static void
_pxpath_to_string(pxpathPtr ptr, struct printbuf *buf) {	
  if(ptr == NULL) return;
	if(ptr->type == PXPATH_FUNCTION) {
		sprintbuf(buf, "%s(", ptr->value);
		pxpathPtr next = ptr->child;
		while(next != NULL) {
			char* comma = (next->next == NULL) ? "" : ", ";
			_pxpath_to_string(next, buf);
			sprintbuf(buf, "%s", comma);
			next = next->next;
		}
		sprintbuf(buf, "%s", ")");
	} else {
		sprintbuf(buf, "%s", ptr->value);
	}
}

static char * 
format_n(int n) {
  char * out = calloc(2 * n + 1, 1);
  for(int i =0; i < n; i++) {
    strcat(out, "%s");
  }
}

char* pxpath_to_string(pxpathPtr ptr) {
  if(ptr == NULL) return NULL;
	struct printbuf *buf = printbuf_new();
	_pxpath_to_string(ptr, buf);
	char *out = strdup(buf->buf);
	free(buf);
	return out;
}

pxpathPtr pxpath_dup(pxpathPtr p) {
  if(p == NULL) return NULL;
  pxpathPtr dup = (pxpathPtr) calloc(sizeof(pxpath_node), 1);
  dup->type = p->type;
  dup->value = strdup(p->value);
  dup->next = pxpath_dup(p->next);
  dup->child = pxpath_dup(p->child);
  return dup;
}

void pxpath_free(pxpathPtr ptr) {
  if(ptr == NULL) return;
  free(ptr->value);
  pxpath_free(ptr->next);
  pxpath_free(ptr->child);
  free(ptr);
}

pxpathPtr pxpath_new_func(char* value, pxpathPtr child) {
	pxpathPtr ptr = pxpath_new(PXPATH_FUNCTION, value);
	ptr->child = child;
	return ptr;
}

pxpathPtr pxpath_cat_things(int n, va_list va) {
	struct printbuf *buf = printbuf_new();
  pxpathPtr last;
  pxpathPtr first;
  for(int i = 0; i < n; i++) {
    pxpathPtr ptr = va_arg(va, pxpathPtr);
    sprintbuf(buf, "%s", pxpath_to_string(ptr));
    if(i == 0) first = ptr;
    if(i != 0) last->next = ptr;
    last = ptr;
  }
  pxpathPtr out = pxpath_new(PXPATH_PATH, buf->buf);
  printbuf_free(buf);
  out->child = first;
  return out;
}

pxpathPtr pxpath_cat_literals(int n, ...) {
	va_list va;
  va_start(va, n);
	pxpathPtr out = pxpath_cat_things(n, va);
	va_end(va);
	out->type = PXPATH_LIT_EXPR;
	return out;
}

pxpathPtr pxpath_cat_paths(int n, ...) {
	va_list va;
  va_start(va, n);
	pxpathPtr out = pxpath_cat_things(n, va);
	va_end(va);
	out->type = PXPATH_PATH;
	return out;
}

static pxpathPtr 
pxpath_new_thing(int n, va_list va) {
	struct printbuf *buf = printbuf_new();
  for(int i = 0; i < n; i++) {
    sprintbuf(buf, "%s", va_arg(va, char *));
  }
  pxpathPtr ptr = pxpath_new(PXPATH_PATH, buf->buf);
  printbuf_free(buf);
  return ptr;
}

pxpathPtr pxpath_new_path(int n, ...) {
  va_list va;
  va_start(va, n);
	pxpathPtr ptr = pxpath_new_thing(n, va);
  va_end(va);
	ptr->type = PXPATH_PATH;
  return ptr;
}

pxpathPtr pxpath_new_literal(int n, ...) {
  va_list va;
  va_start(va, n);
	pxpathPtr ptr = pxpath_new_thing(n, va);
  va_end(va);
	ptr->type = PXPATH_LITERAL;
  return ptr;
}

pxpathPtr pxpath_new_operator(int n, ...) {
  va_list va;
  va_start(va, n);
	pxpathPtr ptr = pxpath_new_thing(n, va);
  va_end(va);
	ptr->type = PXPATH_OPERATOR;
  return ptr;
}
