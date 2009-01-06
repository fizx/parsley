#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include <stdio.h>
#include <json/json.h>

FILE* dex_fopen(char*, char*);
char* sprintbuf_dex_header(struct printbuf *);

#endif