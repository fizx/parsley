#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include <json/json.h>
#include "kstring.h"
#include "obstack.h"
#include "dexter.h"
#include "y.tab.h"
#include "printbuf.h"
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

xmlDocPtr dex_parse_file(dexPtr dex, char* file, boolean html) {
	if(html) {
		htmlParserCtxtPtr htmlCtxt = htmlNewParserCtxt();
  	htmlDocPtr html = htmlCtxtReadFile(htmlCtxt, file, "UTF-8", 3);
		if(html == NULL) {
			asprintf(&dex->error, "Couldn't parse file: %s\n", file);
			return NULL;
		}
		return dex_parse_doc(dex, html);
	} else {
		xmlParserCtxtPtr ctxt = xmlNewParserCtxt();
		xmlDocPtr xml = xmlCtxtReadFile(ctxt, file, "UTF-8", 3);
		if(xml == NULL) {
			asprintf(&dex->error, "Couldn't parse file: %s\n", file);
			return NULL;
		}
		return dex_parse_doc(dex, xml);
	}
}

xmlDocPtr dex_parse_string(dexPtr dex, char* string, size_t size, boolean html) {
	if(html) {
		htmlParserCtxtPtr htmlCtxt = htmlNewParserCtxt();
  	htmlDocPtr html = htmlCtxtReadMemory(htmlCtxt, string, size, "http://foo", "UTF-8", 3);
		if(html == NULL) {
			asprintf(&dex->error, "Couldn't parse string\n");
			return NULL;
		}
		return dex_parse_doc(dex, html);
	} else {
		xmlParserCtxtPtr ctxt = xmlNewParserCtxt();
 		xmlDocPtr xml = xmlCtxtReadMemory(ctxt, string, size, "http://foo", "UTF-8", 3);
		if(xml == NULL) {
			asprintf(&dex->error, "Couldn't parse string\n");
			return NULL;
		}
		return dex_parse_doc(dex, xml);
	}
}

xmlDocPtr dex_parse_doc(dexPtr dex, xmlDocPtr doc) {
	return xsltApplyStylesheet(dex->stylesheet, doc, NULL);
}

dexPtr dex_compile(char* dex_str, char* incl) {
	dexPtr dex = (dexPtr) calloc(sizeof(compiled_dex), 1);
	
	if(last_dex_error != NULL) {
		free(last_dex_error);
		last_dex_error = NULL;
	}
	
  if(!dex_exslt_registered) {
    exsltRegisterAll();
    dex_exslt_registered = true;
  }

	obstack_init(&dex_obstack);
	
	struct json_object *json = json_tokener_parse(dex_str);
	if(is_error(json)) {
		dex->error = strdup("Your dex is not valid json.");
		return dex;
	}

	struct printbuf* buf = printbuf_new();
	
	sprintbuf(buf, "<xsl:stylesheet version=\"1.0\" xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\"");
	sprintbuf(buf, " xmlns:dexter=\"http://kylemaxwell.com/dexter\"");
	sprintbuf(buf, " xmlns:str=\"http://exslt.org/strings\"");
	sprintbuf(buf, " xmlns:set=\"http://exslt.org/sets\"");
	sprintbuf(buf, " xmlns:math=\"http://exslt.org/math\"");
	sprintbuf(buf, " xmlns:func=\"http://exslt.org/functions\"");
	sprintbuf(buf, " xmlns:user=\"http://kylemaxwell.com/dexter/user-functions\"");
	sprintbuf(buf, " xmlns:dyn=\"http://exslt.org/dynamic\"");
	sprintbuf(buf, " xmlns:date=\"http://exslt.org/dates-and-times\"");
	sprintbuf(buf, " xmlns:exsl=\"http://exslt.org/common\"");
	sprintbuf(buf, " xmlns:saxon=\"http://icl.com/saxon\"");
	sprintbuf(buf, " extension-element-prefixes=\"str math set func dyn exsl saxon user date\"");
	sprintbuf(buf, ">\n");
	sprintbuf(buf, "<xsl:output method=\"xml\" indent=\"yes\"/>\n");
	sprintbuf(buf, "<xsl:strip-space elements=\"*\"/>\n");
	sprintbuf(buf, "%s\n", incl);
	sprintbuf(buf, "<xsl:template match=\"/\">\n");
	sprintbuf(buf, "<dexter:root>\n");
		
	contextPtr context = new_context(json, buf);
	__dex_recurse(context);
	
	json_object_put(json); // frees json
	dex->error = last_dex_error;
	
	sprintbuf(buf, "</dexter:root>\n");
	sprintbuf(buf, "</xsl:template>\n");
	sprintbuf(buf, context->key_buf->buf);
	sprintbuf(buf, "</xsl:stylesheet>\n");
	
	if(dex->error == NULL) {
		xmlParserCtxtPtr ctxt = xmlNewParserCtxt();
		xmlDocPtr doc = xmlCtxtReadMemory(ctxt, buf->buf, buf->size, "http://kylemaxwell.com/some-dex", "UTF-8", 3);
		dex->stylesheet = xsltParseStylesheetDoc(doc);
	}
	
	printbuf_free(buf);
	obstack_free(&dex_obstack, NULL);
	
	return dex;
}

contextPtr new_context(struct json_object * json, struct printbuf *buf) {
	contextPtr c = __dex_alloc(sizeof(dex_context));
	c->key_buf = printbuf_new();
	sprintbuf(c->key_buf, "");
	c->name = "root";
	c->tag = "root";
	c->full_expr = "/";
	c->expr = NULL;
	c->magic = NULL;
	c->filter = NULL;
	c->buf = buf;
	c->json = json;
	c->parent = NULL;
	c->array = 0;
	c->string = 0;
	c->keys = NULL;
	return c;
}

contextPtr deeper_context(contextPtr context, char* key, struct json_object * val) {
	contextPtr c = __dex_alloc(sizeof(dex_context));
	c->key_buf = context->key_buf;
	c->keys = context->keys;
	c->tag = __tag(key);
	c->array = json_object_is_type(val, json_type_array);
	c->json = c->array ? json_object_array_get_idx(val, 0) : val;
	c->string = json_object_is_type(c->json, json_type_string);
	c->filter = __filter(key);
	c->name = astrcat3(context->name, ".", c->tag);
	c->magic = ((c->filter == NULL) && c->array && !(c->string)) ? c->name : context->magic;
	c->buf = context->buf;
	c->expr = c->string ? myparse(astrdup(json_object_get_string(c->json))) : NULL;
	c->full_expr = full_expr(context, c->filter);
	c->full_expr = full_expr(c, c->expr);
	c->expr = filter_intersection(c->magic, c->expr);
	c->filter = filter_intersection(c->magic, c->filter);
	c->parent = context;
	return c;
}

static char* filter_intersection(char* key, char* expr) {
	if(key != NULL && expr != NULL) {
		return astrcat7("set:intersection(key('", key, "__key', $", key, "__index), ", expr, ")");
	} else {
		return expr;
	}
}

void dex_free(dexPtr ptr) {
	if(ptr->error != NULL) free(ptr->error);
	if(ptr->stylesheet != NULL) xsltFreeStylesheet(ptr->stylesheet);
	free(ptr);
}

void * __dex_alloc(int size) {
	return obstack_alloc(&dex_obstack, size);
}

void yyerror(const char * s) {
	struct printbuf *buf = printbuf_new();
	if(last_dex_error !=NULL) sprintbuf(buf, "%s\n", last_dex_error);
  sprintbuf(buf, "%s in key: %s", s, dex_parsing_context->name);
	last_dex_error = strdup(buf->buf);
	printbuf_free(buf);
}

char* __tag(char* key) {
	char *tag = astrdup(key);
	char *ptr = tag;
	while(*ptr++ != '\0'){
		if(*ptr == '(') {
			*ptr = 0;
			return tag;
		}
	}
	return tag;
}

char* __filter(char* key) {
	char *expr = astrdup(key);
	char *ptr = expr;

	int offset = 0;
	bool has_expr = false;

	while(*ptr++ != '\0'){
		offset++;
		if(*ptr == '(') {
			has_expr = true;
			break;
		}
	}
	if(!has_expr) return NULL;

	expr += offset + 1; // clip "("
	int l = strlen(expr);
	if(l <= 1) return NULL;
	*(expr + l - 1) = 0; // clip ")"
	return myparse(expr);
}

void __dex_recurse(contextPtr context) {
	char* tmp;
	struct printbuf * buf;
	keyPtr keys;
	contextPtr c;
	json_object_object_foreach(context->json, key, val) {
		c = deeper_context(context, key, val);
		sprintbuf(c->buf, "<%s>\n", c->tag);	
		if(c->string) {
			if(c->array) {
				sprintbuf(c->buf, "<dexter:groups><xsl:for-each select=\"%s\"><dexter:group>\n", c->expr);	
				sprintbuf(c->buf, "<xsl:value-of select=\".\" />\n");
				sprintbuf(c->buf, "</dexter:group></xsl:for-each></dexter:groups>\n");
			} else {
				sprintbuf(c->buf, "<xsl:value-of select=\"%s\" />\n", c->expr);
			} 
		} else { // if c->object !string
			if(c->array) {		// scoped
				if(c->filter != NULL) {
					sprintbuf(c->buf, "<dexter:groups><xsl:for-each select=\"%s\"><dexter:group>\n", c->filter);	
					__dex_recurse(c);
					sprintbuf(c->buf, "</dexter:group></xsl:for-each></dexter:groups>\n");
				} else {				// magic	
					sprintbuf(c->buf, "<xsl:variable name=\"%s__context\" select=\".\"/>\n", c->name);
					tmp = myparse(astrdup(inner_key_of(c->json)));
					sprintbuf(c->buf, "<dexter:groups><xsl:for-each select=\"%s\">\n", filter_intersection(context->magic, tmp));	


					// keys
					keys = __dex_alloc(sizeof(key_node));
					keys->name = c->name;
					keys->use = full_expr(c, tmp);
					keys->next = c->keys;
					c->keys = keys;
					
					buf = printbuf_new();
					
					sprintbuf(buf, "concat(");
					while(keys != NULL){
						sprintbuf(buf, "count(set:intersection(following::*, %s)), '-',", keys->use);
						keys = keys->next;
					}
					sprintbuf(buf, "'')");
					tmp = astrdup(buf->buf);
					printbuf_free(buf);
					
					sprintbuf(c->key_buf, "<xsl:key name=\"%s__key\" match=\"%s\" use=\"%s\"/>\n", c->name, 
						full_expr(c, "./descendant-or-self::*"),
						tmp
					);

					sprintbuf(c->buf, "<xsl:variable name=\"%s__index\" select=\"%s\"/>\n", c->name, tmp);
					sprintbuf(c->buf, "<xsl:for-each select=\"$%s__context\"><dexter:group>\n", c->name);	
					__dex_recurse(c);
					sprintbuf(c->buf, "</dexter:group></xsl:for-each></xsl:for-each></dexter:groups>\n");					
				}
			} else {
				if(c->filter == NULL) {
					__dex_recurse(c);
				} else {	
					sprintbuf(c->buf, "<xsl:for-each select=\"%s\"><xsl:if test=\"position() = 1\">\n", c->filter);	
					__dex_recurse(c);
					sprintbuf(c->buf, "</xsl:if></xsl:for-each>\n");
				}
			}
		}
		sprintbuf(c->buf, "</%s>\n", c->tag);
	}
}

static char* full_expr(contextPtr context, char* expr) {
	if(expr == NULL) return context->full_expr;
	char* merged = arepl(expr, ".", context->full_expr);
	return arepl(merged, "///", "//");
}

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
