#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include <json/json.h>
#include "kstring.h"
#include "obstack.h"
#include "dexter.h"
#include "y.tab.h"
#include "printbuf.h"
#include "functions.h"
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
  	htmlDocPtr html = htmlCtxtReadMemory(htmlCtxt, string, size, "http://kylemaxwell.com/dexter/memory", "UTF-8", 3);
		if(html == NULL) {
			asprintf(&dex->error, "Couldn't parse string\n");
			return NULL;
		}
		return dex_parse_doc(dex, html);
	} else {
		xmlParserCtxtPtr ctxt = xmlNewParserCtxt();
 		xmlDocPtr xml = xmlCtxtReadMemory(ctxt, string, size, "http://kylemaxwell.com/dexter/memory", "UTF-8", 3);
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
		dex_register_all();
		exslt_org_regular_expressions_init();
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
	sprintbuf(buf, " xmlns:dex=\"http://kylemaxwell.com/dexter/library\"");
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
	sprintbuf(buf, " xmlns:regexp=\"http://exslt.org/regular-expressions\"");
	sprintbuf(buf, " extension-element-prefixes=\"dex str math set func dyn exsl saxon user date regexp\"");
	sprintbuf(buf, ">\n");
	sprintbuf(buf, "<xsl:output method=\"xml\" indent=\"yes\"/>\n");
	sprintbuf(buf, "<xsl:strip-space elements=\"*\"/>\n");
	sprintbuf(buf, "<func:function name=\"dex:nl\"><xsl:param name=\"in\" select=\".\"/>");
	sprintbuf(buf, "<xsl:variable name=\"out\"><xsl:apply-templates mode=\"innertext\" select=\"exsl:node-set($in)\"/></xsl:variable>");
	sprintbuf(buf, "<func:result select=\"$out\" /></func:function>");
	sprintbuf(buf, "<xsl:template match=\"text()\" mode=\"innertext\"><xsl:value-of select=\".\" /></xsl:template>");
	sprintbuf(buf, "<xsl:template match=\"script|style\" mode=\"innertext\"/>");
	sprintbuf(buf, "<xsl:template match=\"br|address|blockquote|center|dir|div|form|h1|h2|h3|h4|h5|h6|hr|menu|noframes|noscript|p|pre|li|td|th|p\" mode=\"innertext\"><xsl:apply-templates mode=\"innertext\" /><xsl:text>\n</xsl:text></xsl:template>");
	// sprintbuf(buf, "<xsl:template match=\"node()\" mode=\"innertext\"><xsl:apply-templates mode=\"innertext\" select=\"n\" /></xsl:template>");
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
		xmlDocPtr doc = xmlCtxtReadMemory(ctxt, buf->buf, buf->size, "http://kylemaxwell.com/dexter/compiled", "UTF-8", 3);
		dex->raw_stylesheet = strdup(buf->buf);
		dex->stylesheet = xsltParseStylesheetDoc(doc);
	}
	
	printbuf_free(buf);
	dex_collect();
	
	return dex;
}

void dex_collect() {
	obstack_free(&dex_obstack, NULL);
}

static contextPtr new_context(struct json_object * json, struct printbuf *buf) {
	contextPtr c = dex_alloc(sizeof(dex_context));
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
	// printf("1\n");
	contextPtr c = dex_alloc(sizeof(dex_context));
	c->key_buf = context->key_buf;
	c->keys = context->keys;
	c->tag = dex_key_tag(key);
	
	// printf("4\n");
	c->array = json_object_is_type(val, json_type_array);
	c->json = c->array ? json_object_array_get_idx(val, 0) : val;
	c->string = json_object_is_type(c->json, json_type_string);
	c->filter = dex_key_filter(key);
	
	c->name = astrcat3(context->name, ".", c->tag);
	c->magic = ((c->filter == NULL) && c->array && !(c->string)) ? c->name : context->magic;
	c->buf = context->buf;
	// printf("%d\n", c->string);
	c->expr = c->string ? myparse(astrdup(json_object_get_string(c->json))) : NULL;
	
	// printf("5\n");
	c->full_expr = full_expr(context, c->filter);
	c->full_expr = full_expr(c, c->expr);
	c->expr = filter_intersection(c->magic, c->expr);
	c->filter = filter_intersection(c->magic, c->filter);
	c->parent = context;
	// printf("2\n");
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
	if(ptr->error != NULL) 						
			free(ptr->error);
	if(ptr->raw_stylesheet != NULL)
			free(ptr->raw_stylesheet);
	if(ptr->stylesheet != NULL) 			
			xsltFreeStylesheet(ptr->stylesheet);
	free(ptr);
}

void * dex_alloc(int size) {
	return obstack_alloc(&dex_obstack, size);
}

void yyerror(const char * s) {
	struct printbuf *buf = printbuf_new();
	if(last_dex_error !=NULL) sprintbuf(buf, "%s\n", last_dex_error);
  sprintbuf(buf, "%s in key: %s", s, dex_parsing_context->name);
	last_dex_error = strdup(buf->buf);
	printbuf_free(buf);
}

char* dex_key_tag(char* key) {
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

char* dex_key_filter(char* key) {
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
	// printf("a\n");
	char* tmp;
	struct printbuf * buf;
	keyPtr keys;
	contextPtr c;
	json_object_object_foreach(context->json, key, val) {
		// printf("Y\n");
		c = deeper_context(context, key, val);
		// printf("Z\n");
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
				
				// printf("d\n");
				if(c->filter != NULL) {
					
					// printf("e\n");
					sprintbuf(c->buf, "<dexter:groups><xsl:for-each select=\"%s\"><dexter:group>\n", c->filter);	
					__dex_recurse(c);
					sprintbuf(c->buf, "</dexter:group></xsl:for-each></dexter:groups>\n");
				} else {				// magic	
					
					// printf("f\n");
					sprintbuf(c->buf, "<xsl:variable name=\"%s__context\" select=\".\"/>\n", c->name);
					tmp = myparse(astrdup(inner_key_of(c->json)));
					sprintbuf(c->buf, "<dexter:groups><xsl:for-each select=\"%s\">\n", filter_intersection(context->magic, tmp));	


					// keys
					keys = dex_alloc(sizeof(key_node));
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
				
				// printf("c\n");
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
