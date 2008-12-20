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
#include <json/json.h>
#include <xml2json.h>


VALUE _new(VALUE, VALUE, VALUE);
VALUE _parse_file(VALUE, VALUE, VALUE, VALUE);
VALUE _parse_string(VALUE, VALUE, VALUE, VALUE);
VALUE _parse_doc(xmlDocPtr, VALUE);
VALUE recurse(xmlNodePtr xml);
VALUE c_dex_err;
VALUE c_dex;

void Init_dexterous()
{
	rb_define_class_under(c_dex, "CDexter", rb_cObject);
	rb_define_class_under(c_dex_err, "DexError", rb_eRuntimeError);
	rb_define_static_method(c_dex, "new", _new, 2);
	rb_define_static_method(c_dex, "parse_file", _parse_file, 3);
	rb_define_static_method(c_dex, "parse_string", _parse_string, 3);
}

VALUE _new(VALUE self, VALUE dex, VALUE incl){
	dexPtr dex = dex_compile(STR2CSTR(dex), STR2CSTR(incl));
 	return Data_Wrap_Struct(c_dex, 0, dex_free, dex);
}

VALUE _parse_file(VALUE self, VALUE name, VALUE input, VALUE output){
	dexPtr dex;
	Data_Get_Struct(self, dexPtr, dex);
	return _parse_doc(dex_parse_file(dex, STR2CSTR(name), input == ID2SYM(rb_intern("html"))), output);
}

VALUE _parse_string(VALUE self, VALUE string, VALUE input, VALUE output) {
	dexPtr dex;
	Data_Get_Struct(self, dexPtr, dex);
	char* string = STR2CSTR(name);
	return _parse_doc(dex_parse_string(dex, string, strlen(string), input == ID2SYM(rb_intern("html"))), output);
}

VALUE _parse_doc(xmlDocPtr xml, VALUE type) {
	VALUE output;
	if(type == ID2SYM(rb_intern("json"))) {
		struct json_object *json = xml2json(xml->children->children);
		char* str = json_object_to_json_string(json);
		output = rb_str_new2(str);
		free(str);
		json_object_put(json);
	} else if(type == ID2SYM(rb_intern("xml"))) {
		char* str;
		int size;
		xmlDocDumpMemory(xml, &str, &size);
		output = rb_str_new(str, size);
		free(str);
	} else {
 		output = recurse(xml->children->children);
		if(output == NULL) output = Qnil; 
	}
	return output;
}

VALUE recurse(xmlNodePtr xml) {
  if(xml == NULL) return NULL;
  xmlNodePtr child;
  VALUE obj = Qnil;

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