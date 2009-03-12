#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include <stdio.h>
#include <json/json.h>
#include "parsed_xpath.h"
#include "parsley.h"
#include "parser.h"
#include "regexp.h"
#include "printbuf.h"
#include "functions.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <ctype.h>
#include <libexslt/exslt.h>

FILE* parsley_fopen(char*, char*);
char* sprintbuf_parsley_header(struct printbuf *);
void registerEXSLT();

int parsley_key_flags(char*);
char* parsley_key_tag(char*);
pxpathPtr parsley_key_filter(char*);

#endif