#include "ruby.h"
#include <stdio.h>
#include <libxslt/xslt.h>
#include <libexslt/exslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxml/parser.h>
#include <libxml/HTMLparser.h>
#include <libxml/HTMLtree.h>
#include <libxml/xmlwriter.h>
#include <parsley.h>
#include <json/json.h>
#include <xml2json.h>

VALUE _new(VALUE, VALUE, VALUE);
VALUE _parse_file(VALUE, VALUE, VALUE, VALUE);
VALUE _parse_string(VALUE, VALUE, VALUE, VALUE);
VALUE _parse_doc(parsedParsleyPtr, VALUE);
VALUE rubify_recurse(xmlNodePtr xml);
VALUE c_parsley_err;
VALUE c_parsley;

void Init_cparsley()
{
	c_parsley = rb_define_class("CParsley", rb_cObject);
	c_parsley_err = rb_define_class("ParsleyError", rb_eRuntimeError);
	rb_define_singleton_method(c_parsley, "new", _new, 2);
	rb_define_method(c_parsley, "parse_file", _parse_file, 3);
	rb_define_method(c_parsley, "parse_string", _parse_string, 3);
}

VALUE _new(VALUE self, VALUE parsley, VALUE incl){
	parsleyPtr ptr = parsley_compile(STR2CSTR(parsley), STR2CSTR(incl));
	if(ptr->error != NULL) {
	  rb_raise(c_parsley_err, ptr->error);
    parsley_free(ptr);
    return Qnil;
	}
	
 	return Data_Wrap_Struct(c_parsley, 0, parsley_free, ptr);
}

VALUE _parse_file(VALUE self, VALUE name, VALUE input, VALUE output){
	parsleyPtr parsley;
	Data_Get_Struct(self, parsleyPtr, parsley);
	return _parse_doc(parsley_parse_file(parsley, STR2CSTR(name), input == ID2SYM(rb_intern("html"))), output);
}

VALUE _parse_string(VALUE self, VALUE string, VALUE input, VALUE output) {
	parsleyPtr parsley;
	Data_Get_Struct(self, parsleyPtr, parsley);
	char* cstr = STR2CSTR(string);
	return _parse_doc(parsley_parse_string(parsley, cstr, strlen(cstr), input == ID2SYM(rb_intern("html"))), output);
}

VALUE _parse_doc(parsedParsleyPtr ptr, VALUE type) {
	if(ptr->error != NULL || ptr->xml == NULL) {
    if(ptr->error == NULL) ptr->error = strdup("Unknown parsley error");
		rb_raise(c_parsley_err, ptr->error);
    parsed_parsley_free(ptr);
		return Qnil;
	}
	
	VALUE output;
	if(type == ID2SYM(rb_intern("json"))) {
		struct json_object *json = xml2json(ptr->xml->children->children);
		char* str = json_object_to_json_string(json);
		output = rb_str_new2(str);
		json_object_put(json);
	} else if(type == ID2SYM(rb_intern("xml"))) {
		char* str;
		int size;
		xmlDocDumpMemory(ptr->xml, &str, &size);
		output = rb_str_new(str, size);
	} else {
 		output = rubify_recurse(ptr->xml->children->children);
		if(output == NULL) output = Qnil; 
	}
	
  parsed_parsley_free(ptr);
  
	return output;
}

VALUE rubify_recurse(xmlNodePtr xml) {
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
          rb_hash_aset(obj, rb_str_new2(child->name), rubify_recurse(child->children));
          child = child->next;
        }
      } else if(!strcmp(xml->ns->prefix, "parsley")) {
        if(!strcmp(xml->name, "groups")) {
          obj = rb_ary_new();
          while(child != NULL) {
            rb_ary_push(obj, rubify_recurse(child->children));
            child = child->next;
          }          
        } else if(!strcmp(xml->name, "group")) {
          // Implicitly handled by parsley:groups handler
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