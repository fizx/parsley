#include <libxml/xpath.h>
#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxslt/documents.h>

void dex_register_all();

static void xsltHtmlDocumentFunction(xmlXPathParserContextPtr, int);
static void xsltHtmlDocumentFunctionLoadDocument(xmlXPathParserContextPtr, xmlChar*);
static xsltDocumentPtr	xsltLoadHtmlDocument(xsltTransformContextPtr, const xmlChar *);
static xmlDocPtr xsltHtmlDocLoader(const xmlChar *, xmlDictPtr, int, void *, xsltLoadType type);

