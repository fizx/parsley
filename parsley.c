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
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/HTMLparser.h>
#include <libxml/HTMLtree.h>
#include <libxml/xmlwriter.h>
#include <libxml/debugXML.h>
#include <libexslt/exslt.h>

int yywrap(void){
  return 1;
}

static struct ll {
  xmlChar *name;
  struct ll *next;
};

static char *
full_key_name(contextPtr c) {  
  if(c == NULL || c->parent == NULL) return strdup("/");
  static struct ll * last = NULL;
  while(c->parent != NULL) {
    if(c->tag != NULL) {
      struct ll * ptr = calloc(sizeof(struct ll), 1);
      ptr->name = c->tag;
      ptr->next = last;
      last = ptr;
    }
    c = c->parent;
  }
  struct printbuf *buf = printbuf_new();
  while(last != NULL) {
    sprintbuf(buf, "/%s", last->name);
    last = last->next;
  }
  char *out = strdup(buf->buf);
  printbuf_free(buf);
  return out;
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
      if(xml->prev) xml->prev->next = xml->next;
      if(xml->next) xml->next->prev = xml->prev;
    }    
    sibling = sibling->next;
  }
}

static bool 
is_root(xmlElementPtr xml) {
	return xml != NULL && xml->name != NULL && xml->prefix != NULL && !strcmp(xml->name, "root") && !strcmp(xml->prefix, "parsley");
}

// static bool
// is_root(xmlNodePtr node) {
// 	return !strcmp(node->ns->prefix, "parsley") && !strcmp(node->name, "root");
// }
// 


int compare_pos (const void * a, const void * b)
{
	char* as = xmlGetProp(*(xmlNodePtr*)a, "position");
	char* bs = xmlGetProp(*(xmlNodePtr*)b, "position");
	return atoi(as) - atoi(bs);
}

static void 
_xmlAddChild(xmlNodePtr parent, xmlNodePtr child) {
	xmlNodePtr node = parent->children;
	if(node == NULL) {
		parent->children = child;
		return;
	}
	while(node->next != NULL){
		node = node->next;
	}
	node->next = child;
}

static int 
_xmlChildElementCount(xmlNodePtr n) {
  xmlNodePtr child = n->children;
  int i = 0;
  while(child != NULL) {
    i++;
    child = child->next;
  }
  return i;
}

static void 
collate(xmlNodePtr xml) { 
	// return ;
  if(xml->type != XML_ELEMENT_NODE) return;
  if(xml->ns != NULL && !strcmp(xml->ns->prefix, "parsley") && !strcmp(xml->name, "zipped")){
    xmlNodePtr parent = xml->parent;
    xmlNodePtr child = xml->children;
    int n = _xmlChildElementCount(xml);
    
    xmlNodePtr** name_nodes = malloc(n * sizeof(xmlNodePtr));
    xmlNodePtr* lists = malloc(n * sizeof(xmlNodePtr));
    bool* empty = malloc(n * sizeof(bool));
    bool* multi = malloc(n * sizeof(bool));
    bool* optional = malloc(n * sizeof(bool));
    
		int len = 0;
    for(int i = 0; i < n; i++) {
      name_nodes[i] = xmlCopyNode(child, 2); // 2 means props, ns, no kids
      lists[i] = child->children;
			multi[i] = false;
      optional[i] = xmlGetProp(name_nodes[i], "optional") != NULL;
      if(lists[i] != NULL && !strcmp(lists[i]->name, "groups")) {
        lists[i] = lists[i]->children;
        multi[i] = true;
      }
			lists[i]->parent->extra = i;
			len += _xmlChildElementCount(lists[i]->parent);
      child = child->next;
    }
    xml->children = NULL;

		xmlNodePtr* sortable = malloc(len * sizeof(xmlNodePtr));
		int j = 0;
		
		for(int i = 0; i < n; i++) {
			xmlNodePtr node = lists[i];
			while(node != NULL){
				sortable[j++] = node;
				// printf("%d/%d: %d/%d\n", i, n, j, len);
				node = node->next;
			}
		}
		
		for(int i = 0; i < len; i++) {
			sortable[i]->next = NULL;
		}
		
		qsort(sortable, len, sizeof(xmlNodePtr), compare_pos);
		
		xmlNodePtr groups = xml->parent;
		groups->children = NULL;
		xmlNodePtr group;
		xmlNodePtr* targets = malloc(n * sizeof(xmlNodePtr));
		
		for(j = 0; j < len; j++) {
			int i = sortable[j]->parent->extra;
			if (j == 0 || (!empty[i] && !multi[i] && !optional[i])) { // first or full
				xmlNodePtr group = xmlNewChild(groups, xml->ns, "group", NULL); //new group
				xmlSetProp(group, "optional", "true");
				for(int k = 0; k < n; k++) {
					empty[k] = true;
					targets[k] = xmlCopyNode(name_nodes[k], 2);
					_xmlAddChild(group, targets[k]);
					if(multi[k]) targets[k] = xmlNewChild(targets[k], xml->ns, "groups", NULL);
				}
			}
			
			if(!multi[i]) sortable[j] = sortable[j]->children;
			if(empty[i] || multi[i]) _xmlAddChild(targets[i], sortable[j]);
			empty[i] = false;
		}

    free(name_nodes);
    free(lists);
    free(empty);
    free(multi);
    
    collate(groups);
		// TODO: done? figure out what the recursion target needs to be and reimplement
  } else {
    xmlNodePtr child = xml->children;
    while(child != NULL){
      collate(child);
      child = child->next;
    }
  }
}

static void 
prune(parsedParsleyPtr ptr, xmlNodePtr xml, char* err) {   
	if(xml == NULL) return;
  bool optional = xmlGetProp(xml, "optional") != NULL;
  if(optional) {
    unlink(xml);
    visit(ptr, xml->parent, err);
    return;
  } else {
    if(err == NULL) asprintf(&err, "%s was empty", xpath_of(xml));
    if(!is_root(xml->parent)) {
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
		child->parent = xml;
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
	// printf("hello!\n");
  return true;
}

parsedParsleyPtr parsley_parse_doc(parsleyPtr parsley, xmlDocPtr doc) {
  parsedParsleyPtr ptr = (parsedParsleyPtr) calloc(sizeof(parsed_parsley), 1);
  ptr->error = NULL;
  ptr->parsley = parsley;
  ptr->xml = xsltApplyStylesheet(parsley->stylesheet, doc, NULL);
  if(ptr->xml != NULL && ptr->error == NULL) {
    collate(ptr->xml->children);
    visit(ptr, ptr->xml->children, NULL);
  }
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
	contextPtr c = (contextPtr) calloc(sizeof(parsley_context), 1);
	c->parent = context;
	c->tag = parsley_key_tag(key);
  c->flags = parsley_key_flags(key);
	parsley_parsing_context = c;
	c->array = val != NULL && json_object_is_type(val, json_type_array);
	c->json = c->array ? json_object_array_get_idx(val, 0) : val;
	c->string = val != NULL && json_object_is_type(c->json, json_type_string);
	c->filter = parsley_key_filter(key);
	c->magic = context->array && context->filter == NULL;
	c->buf = context->buf;
	c->expr = c->string ? myparse(json_object_get_string(c->json)) : NULL;
	if(context->child == NULL) {
		context->child = c;
	} else {
		contextPtr tmp = context->child;
		while(tmp->next != NULL) tmp = tmp->next;
		tmp->next = c;
	}
  // fprintf(stderr, "tag:    %s\nexpr:   %s\nfilter: %s\n\n", c->tag, pxpath_to_string(c->expr), pxpath_to_string(c->filter));
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
	return ptr;
}

static char *
inner_path(pxpathPtr p) {
  if(p == NULL) return NULL;
  if(p->type == PXPATH_PATH) return pxpath_to_string(p);
  char *tmp = NULL;
  pxpathPtr ptr = p->child;
  while(ptr != NULL) {
    tmp = inner_path(p->child);
    if(tmp != NULL) return tmp;
    ptr = ptr->next;
  }
  return NULL;
}

//TODO: tree-based replace, instead of naive string-based
static char *
inner_path_to_dot(pxpathPtr p) {
  char *outer = pxpath_to_string(p);
  char *inner = inner_path(p);
  char *repl = NULL; 
  if(inner != NULL) {
    repl = arepl(outer, inner, ".");
    free(inner);
  }
  free(outer);
  return repl;
}

static bool
inner_path_transform(contextPtr c) {
  return c->filter == NULL && c->expr != NULL;
}

static char * 
resolve_filter(contextPtr c) {
  return inner_path_transform(c) ? inner_path(c->expr) : pxpath_to_string(c->filter);
}

static char * 
resolve_expr(contextPtr c) {  
  return inner_path_transform(c) ? inner_path_to_dot(c->expr) : pxpath_to_string(c->expr);
}

static void
render(contextPtr c) {  
  char *filter = resolve_filter(c);
  char *expr = resolve_expr(c);
  char *scope = filter == NULL ? expr : filter;
  bool magic_children = c->array && filter == NULL;
  bool simple_array = c->array && filter != NULL;
  bool filtered = filter != NULL;
  bool multiple = (c->array || c->magic) && !magic_children;
  
  if(c->array)                sprintbuf(c->buf, "<parsley:groups>\n");
  if(filtered)                sprintbuf(c->buf, "<xsl:for-each select=\"%s\">\n", filter);
  if(filtered && !multiple)   sprintbuf(c->buf, "<xsl:if test=\"position()=1\">\n");
  if(multiple)                sprintbuf(c->buf, "<parsley:group optional=\"true\">\n");
  
  sprintbuf(c->buf, "<xsl:attribute name=\"position\"><xsl:value-of select=\"count(preceding::*) + count(ancestor::*)\"/></xsl:attribute>\n");
  
  if(c->string) {
    sprintbuf(c->buf, "<xsl:value-of select=\"%s\" />\n", expr);
  } else {
    if(magic_children)        sprintbuf(c->buf, "<parsley:zipped>\n");
    __parsley_recurse(c);
    if(magic_children)        sprintbuf(c->buf, "</parsley:zipped>\n");
  }
  
  // fprintf(stderr, "d\n");
  
  if(multiple)                sprintbuf(c->buf, "</parsley:group>\n");
  if(filtered && !multiple)   sprintbuf(c->buf, "</xsl:if>\n");
  if(filtered)                sprintbuf(c->buf, "</xsl:for-each>\n");
  if(c->array)                sprintbuf(c->buf, "</parsley:groups>\n");
  
  if(filter !=NULL) free(filter);
  if(expr !=NULL)   free(expr);
}

void __parsley_recurse(contextPtr context) {
	contextPtr c;
	if(context->json == NULL) return;
  // fprintf(stderr, "recursing: %s\n", json_object_to_json_string(context->json));
	json_object_object_foreach(context->json, key, val) {
		c = deeper_context(context, key, val);
		// fprintf(stderr, "<%s%s>\n", c->tag, optional(c));
		sprintbuf(c->buf, "<%s%s>\n", c->tag, optional(c));
    // fprintf(stderr, "a\n");
    render(c);
		sprintbuf(c->buf, "</%s>\n", c->tag);
    // fprintf(stderr, "</%s>\n", c->tag);
	}  
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
