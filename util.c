#include "util.h"

static bool parsley_exslt_registered = false;

#define BUF 128

FILE* parsley_fopen(char* name, char* mode) {
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

void
printbuf_file_read(FILE *f, struct printbuf *buf) {
  char chars[BUF];
  while(fgets(chars, BUF, f) != NULL){
    sprintbuf(buf, "%s", chars);
  }
}

void registerEXSLT() {
  if(!parsley_exslt_registered) {
    exsltRegisterAll();
		parsley_register_all();
		init_xpath_alias();
		exslt_org_regular_expressions_init();
    parsley_exslt_registered = true;
  }
}

int parsley_key_flags(char* key) {
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
      flags |= PARSLEY_OPTIONAL;
    case '!':
      flags |= PARSLEY_BANG;
      break;
    }
  }
  return flags;
}

char* parsley_key_tag(char* key) {
	char *tag = strdup(key);
	char *ptr = tag;
	while(*ptr++ != '\0'){
		if(!isalnum(*ptr) && *ptr != '_' && *ptr != '-') {
			*ptr = 0;
			return tag;
		}
	}
	return tag;
}

pxpathPtr parsley_key_filter(char* key) {
	char *expr = strdup(key);
	char *ptr = expr;
	char *orig = expr;
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

	pxpathPtr out = strlen(expr) == 0 ? NULL : myparse(expr);
	free(orig);
	// free(expr);
	return out;
}

char* sprintbuf_parsley_header(struct printbuf *buf) {
	sprintbuf(buf, "%s", "<xsl:stylesheet version=\"1.0\" xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\"");
	sprintbuf(buf, "%s", " xmlns:lib=\"http://parslets.com/stdlib\"");
	sprintbuf(buf, "%s", " xmlns:parsley=\"http://parslets.com/json\"");
	sprintbuf(buf, "%s", " xmlns:str=\"http://exslt.org/strings\"");
	sprintbuf(buf, "%s", " xmlns:set=\"http://exslt.org/sets\"");
	sprintbuf(buf, "%s", " xmlns:math=\"http://exslt.org/math\"");
	sprintbuf(buf, "%s", " xmlns:func=\"http://exslt.org/functions\"");
	sprintbuf(buf, "%s", " xmlns:user=\"http://parslets.com/usre\"");
	sprintbuf(buf, "%s", " xmlns:dyn=\"http://exslt.org/dynamic\"");
	sprintbuf(buf, "%s", " xmlns:date=\"http://exslt.org/dates-and-times\"");
	sprintbuf(buf, "%s", " xmlns:exsl=\"http://exslt.org/common\"");
	sprintbuf(buf, "%s", " xmlns:saxon=\"http://icl.com/saxon\"");
	sprintbuf(buf, "%s", " xmlns:regexp=\"http://exslt.org/regular-expressions\"");
	sprintbuf(buf, "%s", " xmlns:regex=\"http://exslt.org/regular-expressions\"");
	sprintbuf(buf, "%s", " extension-element-prefixes=\"lib str math set func dyn exsl saxon user date regexp regex\"");
	sprintbuf(buf, "%s", ">\n");
	sprintbuf(buf, "%s", "<xsl:output method=\"xml\" indent=\"yes\"/>\n");
	sprintbuf(buf, "%s", "<xsl:strip-space elements=\"*\"/>\n");
	sprintbuf(buf, "%s", "<func:function name=\"lib:nl\"><xsl:param name=\"in\" select=\".\"/>");
	sprintbuf(buf, "%s", "<xsl:variable name=\"out\"><xsl:apply-templates mode=\"innertext\" select=\"exsl:node-set($in)\"/></xsl:variable>");
	sprintbuf(buf, "%s", "<func:result select=\"$out\" /></func:function>");
	sprintbuf(buf, "%s", "<xsl:template match=\"text()\" mode=\"innertext\"><xsl:value-of select=\".\" /></xsl:template>");
	sprintbuf(buf, "%s", "<xsl:template match=\"script|style\" mode=\"innertext\"/>");
	sprintbuf(buf, "%s", "<xsl:template match=\"br|address|blockquote|center|dir|div|form|h1|h2|h3|h4|h5|h6|hr|menu|noframes|noscript|p|pre|li|td|th|p\" mode=\"innertext\"><xsl:apply-templates mode=\"innertext\" /><xsl:text>\n</xsl:text></xsl:template>");
}
