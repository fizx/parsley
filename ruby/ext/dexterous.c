#include "ruby.h"
#include <libxslt/xslt.h>
#include <libexslt/exslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxml/parser.h>
#include <libxml/HTMLparser.h>
#include <libxml/HTMLtree.h>
#include <libxml/xmlwriter.h>
#include <dexter.h>

VALUE dexCompile(VALUE, VALUE, VALUE);
VALUE dexNative(VALUE, VALUE, VALUE, VALUE);
static char* dexError;
VALUE recurse(xmlNodePtr xml);
void inspect(VALUE);

void inspect(VALUE v) {
  printf("%s\n", STR2CSTR(rb_funcall(v, rb_intern("inspect"), 0)));
}

void Init_dexterous()
{
  VALUE dexterous = rb_define_module("Dexterous");
	rb_define_singleton_method(dexterous, "compile", dexCompile, 2);
	rb_define_singleton_method(dexterous, "parse", dexNative, 3);
}

VALUE dexCompile(VALUE self, VALUE dex, VALUE incl){
	VALUE out;
	char* cdex = STR2CSTR(dex);
	char* cincl = STR2CSTR(incl);
	char* cout = dex_compile(cdex, cincl);
	if(cout !=NULL) {
		out = rb_str_new2(cout);
		free(cout);
	} else{
		out = rb_str_new2(dexError);
	}
	return out;
}

VALUE dexNative(VALUE self, VALUE dex, VALUE rhtml, VALUE incl){
  xmlParserCtxtPtr ctxt = xmlNewParserCtxt();
	char* xslt = STR2CSTR(dexCompile(self, dex, incl));
	xmlDocPtr doc = xmlCtxtReadMemory(ctxt, xslt, strlen(xslt), "http://foo", "UTF-8", 3);
  xsltStylesheetPtr stylesheet = xsltParseStylesheetDoc(doc);
  char* chtml = STR2CSTR(rhtml);
	htmlParserCtxtPtr htmlCtxt = htmlNewParserCtxt();
  htmlDocPtr html = htmlCtxtReadMemory(htmlCtxt, chtml, strlen(chtml), "http://foo", "UTF-8", 3);
  xmlDocPtr xml = xsltApplyStylesheet(stylesheet, html, NULL);
  VALUE v = recurse(xml->children->children);
  rb_global_variable(v);
  return v;
}

void dex_error(char* message) {
	dexError = message;
}

VALUE recurse(xmlNodePtr xml) {
  if(xml == NULL) return NULL;
  xmlNodePtr child;
  VALUE obj;

  switch(xml->type) {
    case XML_ELEMENT_NODE:
      child = xml->children;
      if(xml->ns == NULL) {
        child = xml;
        obj = rb_hash_new();
        while(child != NULL) {
          rb_hash_aset(obj, rb_str_new2(child->name), recurse(child->children));
          child = child->next;
        }
      } else if(!strcmp(xml->ns->prefix, "dexter")) {
        if(!strcmp(xml->name, "groups")) {
          obj = rb_ary_new();
          while(child != NULL) {
            rb_ary_push(obj, recurse(child->children));
            child = child->next;
          }          
        } else if(!strcmp(xml->name, "group")) {
          // Implicitly handled by dexter:groups handler
        }
      }
      break;
    case XML_TEXT_NODE:
      obj = rb_str_new2(xml->content);
      break;
  }
  // inspect(obj);
  return obj;
}