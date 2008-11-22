#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "kstring.h"

char* astrdup(char* c) {
	return strdup(c);
}

char* astrcat(char* a, char* b) {
	char* output = (char*) malloc(sizeof(char) * (strlen(a) + strlen(b) + 1));
	sprintf(output, "%s%s", a, b);
  return output;
}

char* astrcat3(char* a, char* b, char* c) {
	char* output = (char*) malloc(sizeof(char) * (strlen(a) + strlen(b) + strlen(c) + 1));
	sprintf(output, "%s%s%s", a, b, c);
  return output;
}

char* astrcat4(char* a, char* b, char* c, char* d) {
	char* output = (char*) malloc(sizeof(char) * (strlen(a) + strlen(b) + strlen(c) + strlen(d) + 1));
	sprintf(output, "%s%s%s%s", a, b, c, d);
  return output;
}

char* astrcat5(char* a, char* b, char* c, char* d, char* e) {
	char* output = (char*) malloc(sizeof(char) * (strlen(a) + strlen(b) + strlen(c) + strlen(d) + strlen(e) + 1));
	sprintf(output, "%s%s%s%s%s", a, b, c, d, e);
  return output;
}

char* astrcat6(char* a, char* b, char* c, char* d, char* e, char* f) {
	char* output = (char*) malloc(sizeof(char) * (strlen(a) + strlen(b) + strlen(c) + strlen(d) + strlen(e) + strlen(f) + 1));
	sprintf(output, "%s%s%s%s%s%s", a, b, c, d, e, f);
  return output;
}

char* astrcat9(char* a, char* b, char* c, char* d, char* e, char* f, char* g, char* h, char* i) {
	char* output = (char*) malloc(sizeof(char) * (strlen(a) + strlen(b) + strlen(c) + strlen(d) + strlen(e) + strlen(f) + strlen(g) + strlen(h) + strlen(i) + 1));
	sprintf(output, "%s%s%s%s%s%s%s%s%s", a, b, c, d, e, f, g, h, i);
  return output;
}

char* astrcat10(char* a, char* b, char* c, char* d, char* e, char* f, char* g, char* h, char* i, char* j) {
	char* output = (char*) malloc(sizeof(char) * (strlen(a) + strlen(b) + strlen(c) + strlen(d) + strlen(e) + strlen(f) + strlen(g) + strlen(h) + strlen(i) + strlen(j) + 1));
	sprintf(output, "%s%s%s%s%s%s%s%s%s%s", a, b, c, d, e, f, g, h, i, j);
  return output;
}