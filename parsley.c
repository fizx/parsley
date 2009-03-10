#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include <stdarg.h>
#include <json/json.h>
#include "parsley.h"
#include "y.tab.h"
#include "printbuf.h"
#include "functions.h"
#include "util.h"
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <stdbool.h>
#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxml/parser.h>
#include <libxml/HTMLparser.h>
#include <libxml/HTMLtree.h>
#include <libxml/xmlwriter.h>
#include <libexslt/exslt.h>

int yywrap(void){
  return 1;
}

void parsed_parsley_free(parsedParsleyPtr ptr) {
  if(ptr->xml != NULL) xmlFree(ptr->xml);
  if(ptr->error != NULL) free(ptr->error);
  free(ptr);
}

static parsedParsleyPtr parse_error(char* format, ...) {
  parsedParsleyPtr ptr = (parsedParsleyPtr) calloc(sizeof(parsed_parsley), 1);
  ptr->xml = NULL;
	va_list args;
	va_start(args, format);
  vasprintf(&ptr->error, format, args);
	va_end(args);
  return ptr;
}

parsedParsleyPtr parsley_parse_file(parsleyPtr parsley, char* file, bool html) {
	if(html) {
		htmlParserCtxtPtr htmlCtxt = htmlNewParserCtxt();
  	htmlDocPtr html = htmlCtxtReadFile(htmlCtxt, file, NULL, HTML_PARSE_RECOVER | HTML_PARSE_NOERROR |HTML_PARSE_NOWARNING);
    htmlFreeParserCtxt(htmlCtxt);
    if(html == NULL) return parse_error("Couldn't parse file: %s\n", file);
		return parsley_parse_doc(parsley, html);
	} else {
		xmlParserCtxtPtr ctxt = xmlNewParserCtxt();
		xmlDocPtr xml = xmlCtxtReadFile(ctxt, file, NULL, HTML_PARSE_RECOVER | HTML_PARSE_NOERROR |HTML_PARSE_NOWARNING);
		xmlFreeParserCtxt(ctxt);
		if(xml == NULL) return parse_error("Couldn't parse file: %s\n", file);
		return parsley_parse_doc(parsley, xml);
	}
}

parsedParsleyPtr parsley_parse_string(parsleyPtr parsley, char* string, size_t size, bool html) {
	if(html) {
		htmlParserCtxtPtr htmlCtxt = htmlNewParserCtxt();
  	htmlDocPtr html = htmlCtxtReadMemory(htmlCtxt, string, size, "http://parslets.com/in-memory-string", NULL, HTML_PARSE_RECOVER | HTML_PARSE_NOERROR |HTML_PARSE_NOWARNING);
    if(html == NULL) return parse_error("Couldn't parse string");
		return parsley_parse_doc(parsley, html);
	} else {
		xmlParserCtxtPtr ctxt = xmlNewParserCtxt();
 		xmlDocPtr xml = xmlCtxtReadMemory(ctxt, string, size, "http://parslets.com/in-memory-string", NULL, HTML_PARSE_RECOVER | HTML_PARSE_NOERROR |HTML_PARSE_NOWARNING);
		if(xml == NULL) return parse_error("Couldn't parse string");
		return parsley_parse_doc(parsley, xml);
	}
}

static struct ll {
  xmlChar *name;
  struct ll *next;
};

static char *
xpath_of(xmlNodePtr node) {
  if(node == NULL || node->name == NULL && node->parent == NULL) return strdup("/");
  
  struct ll * ptr = (struct ll *) calloc(sizeof(struct ll *), 1);
  
  while(node->name != NULL && node->parent != NULL) {
    if(node->ns == NULL) {
      struct ll * tmp = (struct ll *) calloc(sizeof(struct ll *), 1);
      tmp->name = node->name;
      tmp->next = ptr;
      ptr = tmp;
    }
    node = node->parent;
  }
  
  struct printbuf *buf = printbuf_new();
  while(ptr->name != NULL) {
    sprintbuf(buf, "/%s", ptr->name);
    struct ll * last = ptr;
    ptr = ptr->next;
    free(last);
  }
  free(ptr);
  
  char *str = strdup(buf->buf);
  printbuf_free(buf);
  return str;
}

static void 
unlink(xmlNodePtr xml) {
  if(xml == NULL || xml->parent == NULL) return;
  xmlNodePtr sibling = xml->parent->children;
  if(sibling == xml) {
    xml->parent->children = xml->next;
    return;
  }
  while(sibling != NULL) {
    if(sibling == xml) {
      xml->prev->next = xml->next;
      if(xml->next) xml->next->prev = xml->prev;
    }    
    sibling = sibling->next;
  }
}

static bool 
is_root(xmlElementPtr xml) {
	return xml != NULL && xml->name != NULL && xml->prefix !=NULL && !strcmp(xml->name, "root") && !strcmp(xml->prefix, "parsley");
}

static void 
prune(parsedParsleyPtr ptr, xmlNodePtr xml, char* err) {   
	if(xml == NULL) return;
  bool optional = ((xmlElementPtr )xml)->attributes != NULL;
  if(optional) {
    unlink(xml);
    visit(ptr, xml->parent, err);
    return;
  } else {
    if(err == NULL) asprintf(&err, "%s was empty", xpath_of(xml));
    if(xml->parent != xml->doc->children) {
      prune(ptr, xml->parent, err);
    } else {
      ptr->error = err;
    }
  }
}

static void
visit(parsedParsleyPtr ptr, xmlNodePtr xml, char* err) { 
  if(xml->type != XML_ELEMENT_NODE) return;
  xmlNodePtr child = xml->children;
  xmlNodePtr parent = xml->parent;
  if(parent == NULL) return;
  if(xml_empty(xml)) {
    if(err == NULL) asprintf(&err, "%s was empty", xpath_of(xml));
    prune(ptr, xml, err);
  } else if(err != NULL) {
    free(err);
  }
  while(err == NULL && child != NULL){
    visit(ptr, child, err);
    child = child->next;
  }
}

static bool
xml_empty(xmlNodePtr xml) {  
  xmlNodePtr child = xml->children;
  while(child != NULL) {
    if(child->type != XML_TEXT_NODE) return false;
    if(strlen(child->content)) return false;
    child = child->next;
  }
  return true;
}

parsedParsleyPtr parsley_parse_doc(parsleyPtr parsley, xmlDocPtr doc) {
  parsedParsleyPtr ptr = (parsedParsleyPtr) calloc(sizeof(parsed_parsley), 1);
  ptr->error = NULL;
  ptr->parsley = parsley;
  ptr->xml = xsltApplyStylesheet(parsley->stylesheet, doc, NULL);
  if(ptr->xml != NULL && ptr->error == NULL) visit(ptr, ptr->xml->children, NULL);
  if(ptr->xml == NULL && ptr->error == NULL) { // == NULL
    ptr->error = strdup("Internal runtime error");
  }
	return ptr;
}

parsleyPtr parsley_compile(char* parsley_str, char* incl) {
	parsleyPtr parsley = (parsleyPtr) calloc(sizeof(compiled_parsley), 1);
	
	if(last_parsley_error != NULL) {
		free(last_parsley_error);
		last_parsley_error = NULL;
	}
	
  registerEXSLT();
	
	struct json_object *json = json_tokener_parse(parsley_str);
	if(is_error(json)) {
		parsley->error = strdup("Your parslet is not valid json.");
    // json_object_put(json); // frees json
		return parsley;
	}

	struct printbuf* buf = printbuf_new();
	
  sprintbuf_parsley_header(buf);
	sprintbuf(buf, "%s\n", incl);
	sprintbuf(buf, "<xsl:template match=\"/\">\n");
	sprintbuf(buf, "<parsley:root>\n");
		
	contextPtr context = new_context(json, buf);
	__parsley_recurse(context);
	
	json_object_put(json); // frees json
	parsley->error = last_parsley_error;
	
	sprintbuf(buf, "</parsley:root>\n");
	sprintbuf(buf, "</xsl:template>\n");
	sprintbuf(buf, "</xsl:stylesheet>\n");
	
	if(parsley->error == NULL) {
		xmlParserCtxtPtr ctxt = xmlNewParserCtxt();
		xmlDocPtr doc = xmlCtxtReadMemory(ctxt, buf->buf, buf->size, "http://parslets.com/compiled", NULL, 3);
		xmlFreeParserCtxt(ctxt);
		parsley->raw_stylesheet = strdup(buf->buf);
		parsley->stylesheet = xsltParseStylesheetDoc(doc);
	}
	
	printbuf_free(buf);
	
	return parsley;
}

static contextPtr new_context(struct json_object * json, struct printbuf *buf) {
	contextPtr c = calloc(sizeof(parsley_context), 1);
	c->tag = "root";
	c->expr = pxpath_new_path(1, "/");
	c->buf = buf;
	c->json = json;
	return c;
}

contextPtr deeper_context(contextPtr context, char* key, struct json_object * val) {
	contextPtr c = calloc(sizeof(parsley_context), 1);
	c->tag = parsley_key_tag(key);
  c->flags = parsley_key_flags(key);
	parsley_parsing_context = c;
	c->array = val != NULL && json_object_is_type(val, json_type_array);
	c->json = c->array ? json_object_array_get_idx(val, 0) : val;
	c->string = val != NULL && json_object_is_type(c->json, json_type_string);
	c->filter = parsley_key_filter(key);
	c->magic = context->array && context->filter == NULL;
	c->buf = context->buf;
	c->expr = c->string ? myparse(strdup(json_object_get_string(c->json))) : NULL;
	c->parent = context;
	if(context->child == NULL) {
		context->child = c;
	} else {
		contextPtr tmp = context->child;
		while(tmp->next != NULL) tmp = tmp->next;
		tmp->next = c;
	}
	return c;
}

void context_free(contextPtr c) {
	if(c->tag != NULL) free(c->tag);
	if(c->filter != NULL) pxpath_free(c->filter);
	if(c->expr != NULL) pxpath_free(c->expr);
	if(c->child != NULL) context_free(c->child);
	if(c->next != NULL) context_free(c->next);
	free(c);
}

void parsley_free(parsleyPtr ptr) {
	if(ptr->error != NULL) 						
			free(ptr->error);
	if(ptr->raw_stylesheet != NULL)
			free(ptr->raw_stylesheet);
	if(ptr->stylesheet != NULL) 			
			xsltFreeStylesheet(ptr->stylesheet);
	free(ptr);
}

void yyerror(const char * s) {
	struct printbuf *buf = printbuf_new();
	if(last_parsley_error !=NULL) sprintbuf(buf, "%s\n", last_parsley_error);
  sprintbuf(buf, "%s in key: %s", s, full_key_name(parsley_parsing_context));
	last_parsley_error = strdup(buf->buf);
	printbuf_free(buf);
}

static char* optional(contextPtr c) {
  return (c->flags & PARSLEY_OPTIONAL) ? " optional=\"true\"" : "";
}

static bool 
all_strings(struct json_object * json) {  
	json_object_object_foreach(json, key, val) {
    if(val == NULL || !json_object_is_type(val, json_type_string)) return false;
  }
  return true;
}

void __parsley_recurse(contextPtr context) {
	// printf("a\n");
	char* tmp;
	struct printbuf * buf;
	keyPtr keys;
	contextPtr c;
	if(context->json == NULL) return;
	json_object_object_foreach(context->json, key, val) {
		c = deeper_context(context, key, val);
		sprintbuf(c->buf, "<%s%s>\n", c->tag, optional(c));	
		if(c->string) {
			if(c->array || context->magic) {
				if(c->filter){
      	  // printf("b\n");
					sprintbuf(c->buf, "<parsley:groups optional=\"true\"><xsl:for-each select=\"%s\"><parsley:group optional=\"true\">\n", c->filter);	
					sprintbuf(c->buf, "<xsl:value-of select=\"%s\" />\n", pxpath_to_string(c->expr));
					sprintbuf(c->buf, "</parsley:group></xsl:for-each></parsley:groups>\n");
				} else {
        	// printf("c\n");
					sprintbuf(c->buf, "<parsley:groups optional=\"true\"><xsl:for-each select=\"%s\"><parsley:group optional=\"true\">\n", c->expr);	
					sprintbuf(c->buf, "<xsl:value-of select=\".\" />\n");
					sprintbuf(c->buf, "</parsley:group></xsl:for-each></parsley:groups>\n");
				}
			} else {
				if(c->filter){
      	  // printf("d\n");
					sprintbuf(c->buf, "<xsl:for-each select=\"%s\"><xsl:if test=\"position()=1\">\n", c->filter);	
					sprintbuf(c->buf, "<xsl:value-of select=\"%s\" />\n", pxpath_to_string(c->expr));
					sprintbuf(c->buf, "</xsl:if></xsl:for-each>\n");
				} else {
        	// printf("e\n");
					sprintbuf(c->buf, "<xsl:value-of select=\"%s\" />\n", c->expr);
				}
			} 
		} else { // if c->object !string
			if(c->array) {		// scoped
				if(c->filter != NULL) {
        	// printf("f\n");
					sprintbuf(c->buf, "<parsley:groups optional=\"true\"><xsl:for-each select=\"%s\"><parsley:group optional=\"true\">\n", c->filter);	
					__parsley_recurse(c);
					sprintbuf(c->buf, "</parsley:group></xsl:for-each></parsley:groups>\n");
				} else {				// magic	  
					// if(all_strings(c->json)) {
  					sprintbuf(c->buf, "<parsley:zipped>\n");
            __parsley_recurse(c);
  					sprintbuf(c->buf, "</parsley:zipped>\n");
					// } else {
					//   				  // printf("h\n");
					//   					sprintbuf(c->buf, "<xsl:variable name=\"%s__context\" select=\".\"/>\n", c->name);
					//   					parsley_parsing_context = c;
					//             char * str = inner_key_of(c->json);
					//   					if(str != NULL) {
					//     				  // printf("i\n");
					//     					tmp = myparse(strdup(str));
					//   					  sprintbuf(c->buf, "<parsley:groups optional=\"true\"><xsl:for-each select=\"%s\">\n", filter_intersection(context->magic, tmp));	
					// 
					//     					// keys
					//     					keys = calloc(sizeof(key_node), 1);
					//     					keys->name = c->name;
					//     					keys->use = full_expr(c, tmp);
					//     					keys->next = c->keys;
					//     					c->keys = keys;
					// 
					//     					buf = printbuf_new();
					// 
					//     					sprintbuf(buf, "concat(");
					//     					while(keys != NULL){
					//     						sprintbuf(buf, "count(set:intersection(following::*, %s)), '-',", keys->use);
					//     						keys = keys->next;
					//     					}
					//     					sprintbuf(buf, "'')");
					//     					tmp = strdup(buf->buf);
					//     					printbuf_free(buf);
					// 
					//     					sprintbuf(c->key_buf, "<xsl:key name=\"%s__key\" match=\"%s\" use=\"%s\"/>\n", c->name, 
					//     						full_expr(c, "./descendant-or-self::*"),
					//     						tmp
					//     					);
					// 
					//     					sprintbuf(c->buf, "<xsl:variable name=\"%s__index\" select=\"%s\"/>\n", c->name, tmp);
					//     					sprintbuf(c->buf, "<xsl:for-each select=\"$%s__context\"><parsley:group optional=\"true\">\n", c->name);	
					//     					__parsley_recurse(c);
					//     					sprintbuf(c->buf, "</parsley:group></xsl:for-each></xsl:for-each></parsley:groups>\n");	
					//     				}				
					//   				}
				}
			} else {
				// printf("j\n");
				if(c->filter == NULL) {
					__parsley_recurse(c);
				} else {
				  // printf("k\n");	
					sprintbuf(c->buf, "<xsl:for-each select=\"%s\"><xsl:if test=\"position() = 1\">\n", c->filter);	
					__parsley_recurse(c);
					sprintbuf(c->buf, "</xsl:if></xsl:for-each>\n");
				}
			}
		}
		sprintbuf(c->buf, "</%s>\n", c->tag);
	}
}

static char* 
arepl(char* orig, char* old, char* new) {
	// printf("y\n");
	char* ptr = strdup(orig);
	int nlen = strlen(new);
	int olen = strlen(old);
	char* i;
	struct printbuf * buf = printbuf_new();
	while((i = strstr(ptr, old)) != NULL) {
		printbuf_memappend(buf, ptr, i - ptr);
		printbuf_memappend(buf, new, nlen);
		ptr = i + olen;
	}
	printbuf_memappend(buf, ptr, strlen(ptr));
	ptr = strdup(buf->buf);
	printbuf_free(buf);
	// printf("z\n");
	return ptr;
}

// static char* full_expr(contextPtr context, char* expr) {
// 	if(expr == NULL) return context->full_expr;
// 	char* merged = arepl(expr, ".", context->full_expr);
// 	return arepl(merged, "///", "//");
// }

static char* inner_key_of(struct json_object * json) {
	switch(json_object_get_type(json)) {
		case json_type_string: 
			return json_object_get_string(json);
		case json_type_array:
			return NULL;
		case json_type_object:
			return inner_key_each(json);
	}
}

static char* inner_key_each(struct json_object * json) {
	json_object_object_foreach(json, key, val) {
		char* inner = inner_key_of(val);
		if(inner != NULL) return inner;
	}
	return NULL;
}
