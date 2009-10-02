#include <stdbool.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <libxml/parserInternals.h>
#include <libxml/uri.h>
#include <libxml/xpointer.h>
#include <libxml/xinclude.h>
#include <libxml/HTMLparser.h>
#include <libxml/HTMLtree.h>
#include <libxslt/xslt.h>
#include <libxslt/imports.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/security.h>
#include <libxslt/xsltutils.h>
#include <libxslt/transform.h>
#include <libxslt/extensions.h>
#include <libxslt/documents.h>
#include "functions.h"


void parsley_register_all(){
	xsltRegisterExtModuleFunction ((const xmlChar *) "html-document", "http://parselets.com/stdlib",
		   xsltHtmlDocumentFunction);
  xsltRegisterExtModuleFunction ((const xmlChar *) "outer-xml", "http://parselets.com/stdlib",
		   xsltOuterXmlFunction);
  xsltRegisterExtModuleFunction ((const xmlChar *) "inner-xml", "http://parselets.com/stdlib",
		   xsltInnerXmlFunction);
}

static void 
xsltStarXMLFunction(xmlXPathParserContextPtr ctxt, int nargs, bool is_inner) {
	if (nargs != 1) {
		xsltTransformError(xsltXPathGetTransformContext(ctxt), NULL, NULL,
												"outer-xml() : invalid number of args %d\n",
												nargs);
		ctxt->error = XPATH_INVALID_ARITY;
		return;
	}
	if (ctxt->value->type == XPATH_NODESET) {
		xmlXPathObjectPtr obj, newobj;

		obj = valuePop(ctxt);

		xmlBufferPtr buf = xmlBufferCreate();
		int n = obj->nodesetval->nodeNr;
		for(int i = 0; i < n; i++) {
			xmlNodePtr node = obj->nodesetval->nodeTab[i];
			xmlDocPtr doc = node->doc;
			if(is_inner){
				xmlNodePtr child = node->children;
				while(child != NULL) {
					xmlNodeDump(buf, doc, child, 0, 0);
					child = child->next;
				}
			} else {
				xmlNodeDump(buf, doc, node, 0, 0);
			}
		}
		newobj = xmlXPathNewCString(xmlBufferContent(buf));
		xmlBufferFree(buf);
		valuePush(ctxt, newobj);
	}
}

void
xsltInnerXmlFunction(xmlXPathParserContextPtr ctxt, int nargs) {
	xsltStarXMLFunction(ctxt, nargs, true);
}

void
xsltOuterXmlFunction(xmlXPathParserContextPtr ctxt, int nargs) {
	xsltStarXMLFunction(ctxt, nargs, false);
}

void
xsltHtmlDocumentFunction(xmlXPathParserContextPtr ctxt, int nargs)
{
    xmlXPathObjectPtr obj, obj2 = NULL;
    xmlChar *base = NULL, *URI;


    if ((nargs < 1) || (nargs > 2)) {
        xsltTransformError(xsltXPathGetTransformContext(ctxt), NULL, NULL,
                         "document() : invalid number of args %d\n",
                         nargs);
        ctxt->error = XPATH_INVALID_ARITY;
        return;
    }
    if (ctxt->value == NULL) {
        xsltTransformError(xsltXPathGetTransformContext(ctxt), NULL, NULL,
                         "document() : invalid arg value\n");
        ctxt->error = XPATH_INVALID_TYPE;
        return;
    }

    if (nargs == 2) {
        if (ctxt->value->type != XPATH_NODESET) {
            xsltTransformError(xsltXPathGetTransformContext(ctxt), NULL, NULL,
                             "document() : invalid arg expecting a nodeset\n");
            ctxt->error = XPATH_INVALID_TYPE;
            return;
        }

        obj2 = valuePop(ctxt);
    }

    if (ctxt->value->type == XPATH_NODESET) {
        int i;
        xmlXPathObjectPtr newobj, ret;

        obj = valuePop(ctxt);
        ret = xmlXPathNewNodeSet(NULL);

        if (obj->nodesetval) {
            for (i = 0; i < obj->nodesetval->nodeNr; i++) {
                valuePush(ctxt,
                          xmlXPathNewNodeSet(obj->nodesetval->nodeTab[i]));
                xmlXPathStringFunction(ctxt, 1);
                if (nargs == 2) {
                    valuePush(ctxt, xmlXPathObjectCopy(obj2));
                } else {
                    valuePush(ctxt,
                              xmlXPathNewNodeSet(obj->nodesetval->
                                                 nodeTab[i]));
                }
                xsltHtmlDocumentFunction(ctxt, 2);
                newobj = valuePop(ctxt);
                ret->nodesetval = xmlXPathNodeSetMerge(ret->nodesetval,
                                                       newobj->nodesetval);
                xmlXPathFreeObject(newobj);
            }
        }

        xmlXPathFreeObject(obj);
        if (obj2 != NULL)
            xmlXPathFreeObject(obj2);
        valuePush(ctxt, ret);
        return;
    }
    /*
     * Make sure it's converted to a string
     */
    xmlXPathStringFunction(ctxt, 1);
    if (ctxt->value->type != XPATH_STRING) {
        xsltTransformError(xsltXPathGetTransformContext(ctxt), NULL, NULL,
                         "document() : invalid arg expecting a string\n");
        ctxt->error = XPATH_INVALID_TYPE;
        if (obj2 != NULL)
            xmlXPathFreeObject(obj2);
        return;
    }
    obj = valuePop(ctxt);
    if (obj->stringval == NULL) {
        valuePush(ctxt, xmlXPathNewNodeSet(NULL));
    } else {
        if ((obj2 != NULL) && (obj2->nodesetval != NULL) &&
            (obj2->nodesetval->nodeNr > 0) &&
            IS_XSLT_REAL_NODE(obj2->nodesetval->nodeTab[0])) {
            xmlNodePtr target;

            target = obj2->nodesetval->nodeTab[0];
            if ((target->type == XML_ATTRIBUTE_NODE) ||
	        (target->type == XML_PI_NODE)) {
                target = ((xmlAttrPtr) target)->parent;
            }
            base = xmlNodeGetBase(target->doc, target);
        } else {
            xsltTransformContextPtr tctxt;

            tctxt = xsltXPathGetTransformContext(ctxt);
            if ((tctxt != NULL) && (tctxt->inst != NULL)) {
                base = xmlNodeGetBase(tctxt->inst->doc, tctxt->inst);
            } else if ((tctxt != NULL) && (tctxt->style != NULL) &&
                       (tctxt->style->doc != NULL)) {
                base = xmlNodeGetBase(tctxt->style->doc,
                                      (xmlNodePtr) tctxt->style->doc);
            }
        }
        URI = xmlBuildURI(obj->stringval, base);
        if (base != NULL)
            xmlFree(base);
        if (URI == NULL) {
            valuePush(ctxt, xmlXPathNewNodeSet(NULL));
        } else {
	    xsltHtmlDocumentFunctionLoadDocument( ctxt, URI );
	    xmlFree(URI);
	}
    }
    xmlXPathFreeObject(obj);
    if (obj2 != NULL)
        xmlXPathFreeObject(obj2);
}

static void
xsltHtmlDocumentFunctionLoadDocument(xmlXPathParserContextPtr ctxt, xmlChar* URI)
{
    xsltTransformContextPtr tctxt;
    xmlURIPtr uri;
    xmlChar *fragment;
    xsltDocumentPtr idoc; /* document info */
    xmlDocPtr doc;
    xmlXPathContextPtr xptrctxt = NULL;
    xmlXPathObjectPtr resObj = NULL;

    tctxt = xsltXPathGetTransformContext(ctxt);
    if (tctxt == NULL) {
	xsltTransformError(NULL, NULL, NULL,
	    "document() : internal error tctxt == NULL\n");
	valuePush(ctxt, xmlXPathNewNodeSet(NULL));
	return;
    } 
	
    uri = xmlParseURI((const char *) URI);
    if (uri == NULL) {
	xsltTransformError(tctxt, NULL, NULL,
	    "document() : failed to parse URI\n");
	valuePush(ctxt, xmlXPathNewNodeSet(NULL));
	return;
    } 
    
    /*
     * check for and remove fragment identifier
     */
    fragment = (xmlChar *)uri->fragment;
    if (fragment != NULL) {
        xmlChar *newURI;
	uri->fragment = NULL;
	newURI = xmlSaveUri(uri);
	idoc = xsltLoadHtmlDocument(tctxt, newURI);
	xmlFree(newURI);
    } else
	idoc = xsltLoadHtmlDocument(tctxt, URI);
    xmlFreeURI(uri);
    
    if (idoc == NULL) {
	if ((URI == NULL) ||
	    (URI[0] == '#') ||
	    ((tctxt->style->doc != NULL) &&
	    (xmlStrEqual(tctxt->style->doc->URL, URI)))) 
	{
	    /*
	    * This selects the stylesheet's doc itself.
	    */
	    doc = tctxt->style->doc;
	} else {
	    valuePush(ctxt, xmlXPathNewNodeSet(NULL));

	    if (fragment != NULL)
		xmlFree(fragment);

	    return;
	}
    } else
	doc = idoc->doc;

    if (fragment == NULL) {
	valuePush(ctxt, xmlXPathNewNodeSet((xmlNodePtr) doc));
	return;
    }
	
    /* use XPointer of HTML location for fragment ID */
#ifdef LIBXML_XPTR_ENABLED
    xptrctxt = xmlXPtrNewContext(doc, NULL, NULL);
    if (xptrctxt == NULL) {
	xsltTransformError(tctxt, NULL, NULL,
	    "document() : internal error xptrctxt == NULL\n");
	goto out_fragment;
    }

    resObj = xmlXPtrEval(fragment, xptrctxt);
    xmlXPathFreeContext(xptrctxt);
#endif
    xmlFree(fragment);	

    if (resObj == NULL)
	goto out_fragment;
	
    switch (resObj->type) {
	case XPATH_NODESET:
	    break;
	case XPATH_UNDEFINED:
	case XPATH_BOOLEAN:
	case XPATH_NUMBER:
	case XPATH_STRING:
	case XPATH_POINT:
	case XPATH_USERS:
	case XPATH_XSLT_TREE:
	case XPATH_RANGE:
	case XPATH_LOCATIONSET:
	    xsltTransformError(tctxt, NULL, NULL,
		"document() : XPointer does not select a node set: #%s\n", 
		fragment);
	goto out_object;
    }
    
    valuePush(ctxt, resObj);
    return;

out_object:
    xmlXPathFreeObject(resObj);

out_fragment:
    valuePush(ctxt, xmlXPathNewNodeSet(NULL));
}

xsltDocumentPtr	
xsltLoadHtmlDocument(xsltTransformContextPtr ctxt, const xmlChar *URI) {
    xsltDocumentPtr ret;
    xmlDocPtr doc;

    if ((ctxt == NULL) || (URI == NULL))
	return(NULL);

    /*
     * Security framework check
     */
    if (ctxt->sec != NULL) {
	int res;
	
	res = xsltCheckRead(ctxt->sec, ctxt, URI);
	if (res == 0) {
	    xsltTransformError(ctxt, NULL, NULL,
		 "xsltLoadHtmlDocument: read rights for %s denied\n",
			     URI);
	    return(NULL);
	}
    }

    /*
     * Walk the context list to find the document if preparsed
     */
    ret = ctxt->docList;
    while (ret != NULL) {
	if ((ret->doc != NULL) && (ret->doc->URL != NULL) &&
	    (xmlStrEqual(ret->doc->URL, URI)))
	    return(ret);
	ret = ret->next;
    }

    doc = htmlReadFile(URI, NULL, ctxt->parserOptions | HTML_PARSE_RECOVER | HTML_PARSE_NOERROR |HTML_PARSE_NOWARNING);

    if (doc == NULL)
	return(NULL);

    if (ctxt->xinclude != 0) {
#ifdef LIBXML_XINCLUDE_ENABLED
#if LIBXML_VERSION >= 20603
	xmlXIncludeProcessFlags(doc, ctxt->parserOptions);
#else
	xmlXIncludeProcess(doc);
#endif
#else
	xsltTransformError(ctxt, NULL, NULL,
	    "xsltLoadHtmlDocument(%s) : XInclude processing not compiled in\n",
	                 URI);
#endif
    }
    /*
     * Apply white-space stripping if asked for
     */
    if (xsltNeedElemSpaceHandling(ctxt))
	xsltApplyStripSpaces(ctxt, xmlDocGetRootElement(doc));
    if (ctxt->debugStatus == XSLT_DEBUG_NONE)
	xmlXPathOrderDocElems(doc);

    ret = xsltNewDocument(ctxt, doc);
    return(ret);
}