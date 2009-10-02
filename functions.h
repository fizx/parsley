#ifndef PARSLEY_FUNCTIONS_H_INCLUDED
#define PARSLEY_FUNCTIONS_H_INCLUDED

#include <libxml/xpath.h>
#include <libxml/HTMLparser.h>
#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxslt/documents.h>

void parsley_register_all();

static void xsltHtmlDocumentFunction(xmlXPathParserContextPtr, int);
static void xsltHtmlDocumentFunctionLoadDocument(xmlXPathParserContextPtr, xmlChar*);
static void xsltInnerXmlFunction(xmlXPathParserContextPtr ctxt, int nargs);
static void xsltOuterXmlFunction(xmlXPathParserContextPtr ctxt, int nargs);
xsltDocumentPtr	xsltLoadHtmlDocument(xsltTransformContextPtr, const xmlChar *);

#endif