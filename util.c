#include "util.h"
#include "dexter.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

static bool dex_exslt_registered = false;

FILE* dex_fopen(char* name, char* mode) {
	FILE* fo;
	if(!strcmp("-", name)) {
		if(!strcmp("w", mode)) {
			fo = stdout;
		} else {
			fo = stdin;
		}
	} else {
		fo = fopen(name, mode);
	}
	if(fo == NULL) {		
    fprintf(stderr, "Cannot open file %s, error %d, %s\n", name, errno, strerror(errno));
		exit(1);
	}
	return fo;	
}


void registerEXSLT() {
  if(!dex_exslt_registered) {
    exsltRegisterAll();
		dex_register_all();
		init_xpath_alias();
		exslt_org_regular_expressions_init();
    dex_exslt_registered = true;
  }
}

int dex_key_flags(char* key) {
  char* ptr = key;
  char* last_alnum = key;
  char* last_paren = key;
  while(*ptr++ != '\0'){
    if(isalnum(*ptr)) {
      last_alnum = ptr;
    } else if (*ptr == ')') {
      last_paren = ptr;
    }
  }
  ptr = (last_alnum > last_paren ? last_alnum : last_paren);
  int flags = 0;
  while(*ptr++ != '\0'){
    switch(*ptr){
    case '?':
      flags |= DEX_OPTIONAL;
      break;
    }
  }
  return flags;
}

char* dex_key_tag(char* key) {
	char *tag = astrdup(key);
	char *ptr = tag;
	while(*ptr++ != '\0'){
		if(!isalnum(*ptr) && *ptr != '_' && *ptr != '-') {
			*ptr = 0;
			return tag;
		}
	}
	return tag;
}

char* dex_key_filter(char* key) {
	char *expr = astrdup(key);
	char *ptr = expr;
  char *last_paren;

	int offset = 0;
	bool has_expr = false;

	while(*ptr++ != '\0'){
		if(!has_expr)     offset++;
		if(*ptr == '(')   has_expr = true;
    if(*ptr == ')')   last_paren = ptr;
	}
	if(!has_expr) return NULL;
  *last_paren = 0; // clip ")"
	expr += offset + 1; // clip "("
  
	return strlen(expr) == 0 ? NULL : myparse(expr);
}



char* sprintbuf_dex_header(struct printbuf *buf) {
	sprintbuf(buf, "<xsl:stylesheet version=\"1.0\" xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\"");
	sprintbuf(buf, " xmlns:dex=\"http://kylemaxwell.com/dexter/library\"");
	sprintbuf(buf, " xmlns:dexter=\"http://kylemaxwell.com/dexter\"");
	sprintbuf(buf, " xmlns:str=\"http://exslt.org/strings\"");
	sprintbuf(buf, " xmlns:set=\"http://exslt.org/sets\"");
	sprintbuf(buf, " xmlns:math=\"http://exslt.org/math\"");
	sprintbuf(buf, " xmlns:func=\"http://exslt.org/functions\"");
	sprintbuf(buf, " xmlns:user=\"http://kylemaxwell.com/dexter/user-functions\"");
	sprintbuf(buf, " xmlns:dyn=\"http://exslt.org/dynamic\"");
	sprintbuf(buf, " xmlns:date=\"http://exslt.org/dates-and-times\"");
	sprintbuf(buf, " xmlns:exsl=\"http://exslt.org/common\"");
	sprintbuf(buf, " xmlns:saxon=\"http://icl.com/saxon\"");
	sprintbuf(buf, " xmlns:regexp=\"http://exslt.org/regular-expressions\"");
	sprintbuf(buf, " xmlns:regex=\"http://exslt.org/regular-expressions\"");
	sprintbuf(buf, " extension-element-prefixes=\"dex str math set func dyn exsl saxon user date regexp regex\"");
	sprintbuf(buf, ">\n");
	sprintbuf(buf, "<xsl:output method=\"xml\" indent=\"yes\"/>\n");
	sprintbuf(buf, "<xsl:strip-space elements=\"*\"/>\n");
	sprintbuf(buf, "<func:function name=\"dex:nl\"><xsl:param name=\"in\" select=\".\"/>");
	sprintbuf(buf, "<xsl:variable name=\"out\"><xsl:apply-templates mode=\"innertext\" select=\"exsl:node-set($in)\"/></xsl:variable>");
	sprintbuf(buf, "<func:result select=\"$out\" /></func:function>");
	sprintbuf(buf, "<xsl:template match=\"text()\" mode=\"innertext\"><xsl:value-of select=\".\" /></xsl:template>");
	sprintbuf(buf, "<xsl:template match=\"script|style\" mode=\"innertext\"/>");
	sprintbuf(buf, "<xsl:template match=\"br|address|blockquote|center|dir|div|form|h1|h2|h3|h4|h5|h6|hr|menu|noframes|noscript|p|pre|li|td|th|p\" mode=\"innertext\"><xsl:apply-templates mode=\"innertext\" /><xsl:text>\n</xsl:text></xsl:template>");
}
