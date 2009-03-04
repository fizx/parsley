#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "kstring.h"
#include "printbuf.h"
#include "parsley_mem.h"
#include "parsley.h"

char* arepl(char* orig, char* old, char* new) {
	// printf("y\n");
	char* ptr = astrdup(orig);
	int nlen = strlen(new);
	int olen = strlen(old);
	char* i;
	struct printbuf * buf = printbuf_new();
	while((i = strstr(ptr, old)) != NULL) {
		printbuf_memappend(buf, ptr, i - ptr);
		printbuf_memappend(buf, new, nlen);
		ptr = i + olen;
	}
	printbuf_memappend(buf, ptr, strlen(ptr));
	ptr = astrdup(buf->buf);
	printbuf_free(buf);
	// printf("z\n");
	return ptr;
}

char* astrdup(char* c) {
	if(c == NULL) return NULL;
	return astrcat(c, "");
}

char* astrcat(char* a, char* b) {
	char* output = (char*) parsley_alloc(sizeof(char) * (strlen(a) + strlen(b) + 1));
	sprintf(output, "%s%s", a, b);
  return output;
}

char* astrcat3(char* a, char* b, char* c) {
	char* output = (char*) parsley_alloc(sizeof(char) * (strlen(a) + strlen(b) + strlen(c) + 1));
	sprintf(output, "%s%s%s", a, b, c);
  return output;
}

char* astrcat4(char* a, char* b, char* c, char* d) {
	char* output = (char*) parsley_alloc(sizeof(char) * (strlen(a) + strlen(b) + strlen(c) + strlen(d) + 1));
	sprintf(output, "%s%s%s%s", a, b, c, d);
  return output;
}

char* astrcat5(char* a, char* b, char* c, char* d, char* e) {
	char* output = (char*) parsley_alloc(sizeof(char) * (strlen(a) + strlen(b) + strlen(c) + strlen(d) + strlen(e) + 1));
	sprintf(output, "%s%s%s%s%s", a, b, c, d, e);
  return output;
}

char* astrcat6(char* a, char* b, char* c, char* d, char* e, char* f) {
	char* output = (char*) parsley_alloc(sizeof(char) * (strlen(a) + strlen(b) + strlen(c) + strlen(d) + strlen(e) + strlen(f) + 1));
	sprintf(output, "%s%s%s%s%s%s", a, b, c, d, e, f);
  return output;
}

char* astrcat7(char* a, char* b, char* c, char* d, char* e, char* f, char* g) {
	char* output = (char*) parsley_alloc(sizeof(char) * (strlen(a) + strlen(b) + strlen(c) + strlen(d) + strlen(e) + strlen(f) + strlen(g) + 1));
	sprintf(output, "%s%s%s%s%s%s%s", a, b, c, d, e, f, g);
  return output;
}

char* astrcat8(char* a, char* b, char* c, char* d, char* e, char* f, char* g, char* h) {
	char* output = (char*) parsley_alloc(sizeof(char) * (strlen(a) + strlen(b) + strlen(c) + strlen(d) + strlen(e) + strlen(f) + strlen(g) + strlen(h) + 1));
	sprintf(output, "%s%s%s%s%s%s%s%s", a, b, c, d, e, f, g, h);
  return output;
}

char* astrcat9(char* a, char* b, char* c, char* d, char* e, char* f, char* g, char* h, char* i) {
	char* output = (char*) parsley_alloc(sizeof(char) * (strlen(a) + strlen(b) + strlen(c) + strlen(d) + strlen(e) + strlen(f) + strlen(g) + strlen(h) + strlen(i) + 1));
	sprintf(output, "%s%s%s%s%s%s%s%s%s", a, b, c, d, e, f, g, h, i);
  return output;
}

char* astrcat10(char* a, char* b, char* c, char* d, char* e, char* f, char* g, char* h, char* i, char* j) {
	char* output = (char*) parsley_alloc(sizeof(char) * (strlen(a) + strlen(b) + strlen(c) + strlen(d) + strlen(e) + strlen(f) + strlen(g) + strlen(h) + strlen(i) + strlen(j) + 1));
	sprintf(output, "%s%s%s%s%s%s%s%s%s%s", a, b, c, d, e, f, g, h, i, j);
  return output;
}