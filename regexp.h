#ifndef REGEXP_H_INCLUDED
#define REGEXP_H_INCLUDED

#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include <libxslt/xsltconfig.h>
#include <libxslt/xsltutils.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/extensions.h>
#include <libexslt/exsltexports.h>

#include <pcre.h>
#include <string.h>

/* make sure init function is exported on win32 */
#if defined(_WIN32)
  #define PLUGINPUBFUN __declspec(dllexport)
#else
  #define PLUGINPUBFUN
#endif

/**
 * EXSLT_REGEXP_NAMESPACE:
 *
 * Namespace for EXSLT regexp functions
 */
#define EXSLT_REGEXP_NAMESPACE ((const xmlChar *) "http://exslt.org/regular-expressions")

void
PLUGINPUBFUN exslt_org_regular_expressions_init (void);



#endif