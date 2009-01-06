#include "util.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>


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
	sprintbuf(buf, " extension-element-prefixes=\"dex str math set func dyn exsl saxon user date regexp\"");
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
