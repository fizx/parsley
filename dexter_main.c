#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "kstring.h"
#include "printbuf.h"
#include "dexter.h"
#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxml/parser.h>
#include <libxml/HTMLparser.h>
#include <libxml/HTMLtree.h>
#include <libxml/xmlwriter.h>

void dex_error(char* msg){
	fprintf(stderr, "%s\n", msg);
  exit(1);
}

int main (int argc, char **argv) {
  if(argc != 3) {
    printf("Usage: dexter DEX HTML\n");
    exit(1);
  }
  struct printbuf *dex = printbuf_new();
  
  FILE * fd = fopen(argv[1], "r");
  
  printbuf_file_read(fd, dex);
  
  xmlParserCtxtPtr ctxt = xmlNewParserCtxt();
  char* xslt = dex_compile(dex->buf, "");
  xmlDocPtr doc = xmlCtxtReadMemory(ctxt, xslt, strlen(xslt), "http://foo", "UTF-8", 3);
  xsltStylesheetPtr stylesheet = xsltParseStylesheetDoc(doc);
  
  htmlParserCtxtPtr htmlCtxt = htmlNewParserCtxt();
  htmlDocPtr html = htmlCtxtReadFile(htmlCtxt, argv[2], "UTF-8", 3);
  
  xmlDocPtr out = xsltApplyStylesheet(stylesheet, html, NULL);
  
  xmlSaveFormatFileEnc("-", out, "UTF-8", 1);
  
	return 0;
}