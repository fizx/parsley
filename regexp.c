/*
 * regexp.c: Implementation of the EXSLT -- Regular Expressions module
 *
 * References:
 *   http://exslt.org/regexp/index.html
 *
 * See Copyright for the status of this software.
 *
 * Authors:
 *   Joel W. Reed <joelwreed@gmail.com>
 *   Some modification by Kyle Maxwell
 *
 * TODO:
 * functions:
 *   regexp:match
 *   regexp:replace
 *   regexp:test
 */
#include "regexp.h"

static void
exsltRegexpFlagsFromString(const xmlChar* flagstr, 
                           int* global, int* flags)
{
  const xmlChar* i = flagstr;

  /* defaults */
  (*flags) = PCRE_UTF8;
  (*global) =  0;

  while (*i != '\0')
    {
      if (*i == 'i') (*flags) |= PCRE_CASELESS;
      else if (*i == 'g') (*global)= 1;
      /* TODO: support other flags? */
      i++;
    }
}

static int
exsltRegexpExecute(xmlXPathParserContextPtr ctxt, 
                   const xmlChar* haystack, const xmlChar* regexp,
                   int flags, int ovector[], int ovector_len)
{
  int haystack_len = 0;
  pcre *compiled_regexp = NULL;
  int rc = 0, erroffset = 0;
  const char *error = 0;

  compiled_regexp = pcre_compile(regexp,      /* the pattern */
                                 flags,       /* default options */
                                 &error,      /* for error message */
                                 &erroffset,  /* for error offset */
                                 NULL);       /* use default character tables */

  if (compiled_regexp == NULL) {
    xsltTransformError (xsltXPathGetTransformContext (ctxt), NULL, NULL,
                        "exslt:regexp failed to compile %s (char: %d). %s", regexp, erroffset, error);
    return -1;
  }

  haystack_len = xmlUTF8Strlen (haystack);

  rc = pcre_exec(compiled_regexp,               /* result of pcre_compile() */
                 NULL,                          /* we didn't study the pattern */
                 haystack,                      /* the subject string */
                 haystack_len,                  /* the length of the subject string */
                 0,                             /* start at offset 0 in the subject */
                 0,                             /* default options */
                 (int*)ovector,                       /* vector of integers for substring information */
                 ovector_len);  /* number of elements in the vector  (NOT size in bytes) */

  if (rc < -1) {
    xsltTransformError (xsltXPathGetTransformContext (ctxt), NULL, NULL,
                        "exslt:regexp failed to execute %s for %s", regexp, haystack);
    rc = 0;
  }
  
  if (compiled_regexp != NULL) 
    pcre_free(compiled_regexp);

  return rc;
}

/**
 * exsltRegexpMatchFunction:
 * @ns:     
 *
 * Returns a node set of string matches
 */

static void
exsltRegexpMatchFunction (xmlXPathParserContextPtr ctxt, int nargs)
{
    xsltTransformContextPtr tctxt;
    xmlNodePtr node;
    xmlDocPtr container;
    xmlXPathObjectPtr ret = NULL;
    xmlChar *haystack, *regexp, *flagstr, *working, *match;
    int rc, x, flags, global, ovector[30];

    if ((nargs < 1) || (nargs > 3)) {
        xmlXPathSetArityError(ctxt);
        return;
    }


    if (nargs > 2) {
      flagstr = xmlXPathPopString(ctxt);
      if (xmlXPathCheckError(ctxt) || (flagstr == NULL)) {
          return;
      }
    } else {
     flagstr = xmlStrdup("");
    }
    
    regexp = xmlXPathPopString(ctxt);
    if (xmlXPathCheckError(ctxt) || (regexp == NULL)) {
        xmlFree(flagstr);
        return;
    }

    haystack = xmlXPathPopString(ctxt);
    if (xmlXPathCheckError(ctxt) || (haystack == NULL)) {
        xmlFree(regexp);
        xmlFree(flagstr);
        return;
    }

    /* Return a result tree fragment */
    tctxt = xsltXPathGetTransformContext(ctxt);
    if (tctxt == NULL) {
      xsltTransformError(xsltXPathGetTransformContext(ctxt), NULL, NULL,
                         "exslt:regexp : internal error tctxt == NULL\n");
      goto fail;
    }

    container = xsltCreateRVT(tctxt);
    if (container != NULL) {
      xsltRegisterTmpRVT(tctxt, container);
      ret = xmlXPathNewNodeSet(NULL);
      if (ret != NULL) {
        ret->boolval = 0; 

        exsltRegexpFlagsFromString(flagstr, &global, &flags);
        working = haystack;
        rc = exsltRegexpExecute(ctxt, working, regexp, flags, 
                                ovector, sizeof(ovector)/sizeof(int));

        while (rc > 0) {
					for(int group = 0; group < rc; group++) {
          	match = xmlStrsub(working, ovector[group*2], ovector[group*2+1]-ovector[group*2]);
          	if (NULL == match) goto fail;

	          node = xmlNewDocRawNode(container, NULL, "match", match);
	          xmlFree(match);

	          xmlAddChild((xmlNodePtr) container, node);
	          xmlXPathNodeSetAddUnique(ret->nodesetval, node);
					}
          if (!global) break;

          working = working + ovector[1];
          rc = exsltRegexpExecute(ctxt, working, regexp, flags, 
                                  ovector, sizeof(ovector)/sizeof(int));
        }
      }
    }
    
 fail:
    if (flagstr != NULL)
      xmlFree(flagstr);
    if (regexp != NULL)
      xmlFree(regexp);
    if (haystack != NULL)
      xmlFree(haystack);

    if (ret != NULL)
      valuePush(ctxt, ret);
    else
      valuePush(ctxt, xmlXPathNewNodeSet(NULL));
}

/**
 * exsltRegexpReplaceFunction:
 * @ns:     
 *
 * Returns a node set of string matches
 */

static void
exsltRegexpReplaceFunction (xmlXPathParserContextPtr ctxt, int nargs)
{
    xmlChar *haystack, *regexp, *flagstr, *replace, *tmp;
    xmlChar *result = NULL, *working, *end;
    int rc, x, flags, global, ovector[3];

    if ((nargs < 1) || (nargs > 4)) {
        xmlXPathSetArityError(ctxt);
        return;
    }

    replace = xmlXPathPopString(ctxt);
    if (xmlXPathCheckError(ctxt) || (replace == NULL)) {
        return;
    }

    flagstr = xmlXPathPopString(ctxt);
    if (xmlXPathCheckError(ctxt) || (flagstr == NULL)) {
        xmlFree(replace);
        return;
    }

    regexp = xmlXPathPopString(ctxt);
    if (xmlXPathCheckError(ctxt) || (regexp == NULL)) {
        xmlFree(flagstr);
        xmlFree(replace);
        return;
    }

    haystack = xmlXPathPopString(ctxt);
    if (xmlXPathCheckError(ctxt) || (haystack == NULL)) {
        xmlFree(regexp);
        xmlFree(flagstr);
        xmlFree(replace);
        return;
    }

    exsltRegexpFlagsFromString(flagstr, &global, &flags);

    working = haystack;
    rc = exsltRegexpExecute(ctxt, working, regexp, flags, 
                            ovector, sizeof(ovector)/sizeof(int));

    while (rc > 0 ) {
      if (0==ovector[0]) {
        if (NULL==result) result = xmlStrdup(replace);
        else result = xmlStrcat(result, replace);
      }
      else {
        tmp = xmlStrsub(working, 0, ovector[0]);
        if (NULL==result) result = tmp;
        else {
          result = xmlStrcat(result, tmp);
          xmlFree(tmp);
        }
        result = xmlStrcat(result, replace);
      }
      
      working = working + ovector[1];

      if (!global) break;
      rc = exsltRegexpExecute(ctxt, working, regexp, flags, 
                              ovector, sizeof(ovector)/sizeof(int));
    }

    end = haystack + xmlUTF8Strlen(haystack);
    if (working < end ) {
        if (NULL==result) result = xmlStrdup(working);
        else {
          result = xmlStrcat(result, working);
        }
    }

fail:
    if (replace != NULL)
            xmlFree(replace);
    if (flagstr != NULL)
            xmlFree(flagstr);
    if (regexp != NULL)
            xmlFree(regexp);
    if (haystack != NULL)
            xmlFree(haystack);

    xmlXPathReturnString(ctxt, result);
}

/**
 * exsltRegexpTestFunction:
 * @ns:     
 *
 * returns true if the string given as the first argument 
 * matches the regular expression given as the second argument
 * 
 */

static void
exsltRegexpTestFunction (xmlXPathParserContextPtr ctxt, int nargs)
{
    xmlChar *haystack, *regexp_middle, *regexp, *flagstr;
    int rc = 0, flags, global, ovector[3];

    if ((nargs < 1) || (nargs > 3)) {
        xmlXPathSetArityError(ctxt);
        return;
    }

    if(nargs > 2) {
    flagstr = xmlXPathPopString(ctxt);
      if (xmlXPathCheckError(ctxt) || (flagstr == NULL)) {
          return;
      }
    } else {
      flagstr = xmlStrdup("");
    }

    regexp_middle = xmlXPathPopString(ctxt);
    if (xmlXPathCheckError(ctxt) || (regexp_middle == NULL)) {
        xmlFree(flagstr);
        return;
    }

    haystack = xmlXPathPopString(ctxt);
    if (xmlXPathCheckError(ctxt) || (haystack == NULL)) {
        xmlFree(regexp_middle);
        xmlFree(flagstr);
        return;
    }

    /* build the regexp */
    regexp = xmlStrdup("\\A");
    regexp = xmlStrcat(regexp, regexp_middle);
    regexp = xmlStrcat(regexp, "\\Z");

    exsltRegexpFlagsFromString(flagstr, &global, &flags);
    rc = exsltRegexpExecute(ctxt, haystack, regexp, flags, 
                            ovector, sizeof(ovector)/sizeof(int));

fail:
    if (flagstr != NULL)
            xmlFree(flagstr);
    if (regexp != NULL)
            xmlFree(regexp);
    if (regexp_middle != NULL)
            xmlFree(regexp_middle);
    if (haystack != NULL)
            xmlFree(haystack);

    xmlXPathReturnBoolean(ctxt, (rc > 0));
}

/**
 * exsltRegexpRegister:
 *
 * Registers the EXSLT - Regexp module
 */
void
PLUGINPUBFUN exslt_org_regular_expressions_init (void)
{
    xsltRegisterExtModuleFunction ((const xmlChar *) "match",
                                   (const xmlChar *) EXSLT_REGEXP_NAMESPACE,
                                   exsltRegexpMatchFunction);
    xsltRegisterExtModuleFunction ((const xmlChar *) "replace",
                                   (const xmlChar *) EXSLT_REGEXP_NAMESPACE,
                                   exsltRegexpReplaceFunction);
    xsltRegisterExtModuleFunction ((const xmlChar *) "test",
                                   (const xmlChar *) EXSLT_REGEXP_NAMESPACE,
                                   exsltRegexpTestFunction);
}
