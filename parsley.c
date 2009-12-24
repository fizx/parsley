#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include <stdarg.h>
#include <json/json.h>
#include "parsley.h"
#include "y.tab.h"
#include <json/printbuf.h>
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
#include <libxml/debugXML.h>
#include <libxml/HTMLparser.h>
#include <libxml/HTMLtree.h>
#include <libxml/xmlwriter.h>
#include <libxml/xmlerror.h>
#include <libxml/debugXML.h>
#include <libexslt/exslt.h>

int yywrap(void){
  return 1;
}

struct ll {
  xmlChar *name;
  struct ll *next;
};

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

parsedParsleyPtr parsley_parse_file(parsleyPtr parsley, char* file, int flags) {
  xmlSetGenericErrorFunc(NULL , parsleyXsltError);
	bool html = flags & PARSLEY_OPTIONS_HTML;
  char * encoding = flags & PARSLEY_OPTIONS_FORCE_UTF8 ? "UTF-8" : NULL;
	if(html) {
		htmlParserCtxtPtr htmlCtxt = htmlNewParserCtxt();
  	htmlDocPtr html = htmlCtxtReadFile(htmlCtxt, file, encoding, HTML_PARSE_RECOVER | HTML_PARSE_NOERROR |HTML_PARSE_NOWARNING);
    htmlFreeParserCtxt(htmlCtxt);
    if(html == NULL) return parse_error("Couldn't parse file: %s\n", file);
    parsedParsleyPtr out = parsley_parse_doc(parsley, html, flags);
    xmlFreeDoc(html);
    return out;
	} else {
		xmlParserCtxtPtr ctxt = xmlNewParserCtxt();
		xmlDocPtr xml = xmlCtxtReadFile(ctxt, file, encoding, HTML_PARSE_RECOVER | HTML_PARSE_NOERROR |HTML_PARSE_NOWARNING);
		xmlFreeParserCtxt(ctxt);
		if(xml == NULL) return parse_error("Couldn't parse file: %s\n", file);
    parsedParsleyPtr out = parsley_parse_doc(parsley, xml, flags);
    xmlFreeDoc(xml);
    return out;
	}
}

parsedParsleyPtr parsley_parse_string(parsleyPtr parsley, char* string, size_t size, char* base_uri, int flags) {
  xmlSetGenericErrorFunc(NULL , parsleyXsltError);
	bool html = flags & PARSLEY_OPTIONS_HTML;
  char * encoding = flags & PARSLEY_OPTIONS_FORCE_UTF8 ? "UTF-8" : NULL;
	if(base_uri == NULL) base_uri = "http://parselets.com/in-memory-string";
	if(html) {
		htmlParserCtxtPtr htmlCtxt = htmlNewParserCtxt();
  	htmlDocPtr html = htmlCtxtReadMemory(htmlCtxt, string, size, base_uri, encoding, HTML_PARSE_RECOVER | HTML_PARSE_NOERROR |HTML_PARSE_NOWARNING);
    if(html == NULL) return parse_error("Couldn't parse string");
    parsedParsleyPtr out = parsley_parse_doc(parsley, html, flags);
    xmlFreeDoc(html);
    return out;
	} else {
		xmlParserCtxtPtr ctxt = xmlNewParserCtxt();
 		xmlDocPtr xml = xmlCtxtReadMemory(ctxt, string, size, base_uri, encoding, HTML_PARSE_RECOVER | HTML_PARSE_NOERROR |HTML_PARSE_NOWARNING);
		if(xml == NULL) return parse_error("Couldn't parse string");
    parsedParsleyPtr out = parsley_parse_doc(parsley, xml, flags);
    xmlFreeDoc(xml);
    return out;
	}
}

static char *
xpath_of(xmlNodePtr node) {
  if(node == NULL || node->name == NULL || node->parent == NULL) return strdup("/");
  
  struct ll * ptr = (struct ll *) calloc(sizeof(struct ll), 1);
  
  while(node->name != NULL && node->parent != NULL) {
    if(node->ns == NULL) {
      struct ll * tmp = (struct ll *) calloc(sizeof(struct ll), 1);
      tmp->name = node->name;
      tmp->next = ptr;
      ptr = tmp;
    }
    node = node->parent;
  }
  
  struct printbuf *buf = printbuf_new();
  sprintbuf(buf, "");
  while(ptr->name != NULL) {
    sprintbuf(buf, "/%s", ptr->name);
    struct ll * last = ptr;
    ptr = ptr->next;
    free(last);
  }
  free(ptr);
  
  char *str = strdup(strlen(buf->buf) ? buf->buf : "/");
  printbuf_free(buf);
  return str;
}

static bool
is_root(xmlNodePtr node) {
  return node != NULL && node->ns != NULL && !strcmp(node->ns->prefix, "parsley") && !strcmp(node->name, "root");
}

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

static bool
xml_empty(xmlNodePtr xml) { 
  // fprintf(stderr, "%s\n", xml->name);
  xmlNodePtr child = xml->children;
  while(child != NULL) {
    if(child->type != XML_TEXT_NODE) return false;
    if(strlen(child->content)) return false;
    child = child->next;
  }
  // printf("hello!\n");
  return true;
}

static void 
collate(xmlNodePtr xml) { 
	if(xml == NULL) return ;
  if(xml->type != XML_ELEMENT_NODE) return;
  if(xml->ns != NULL && !strcmp(xml->ns->prefix, "parsley") && !strcmp(xml->name, "zipped")){
    xmlNodePtr parent = xml->parent;
    xmlNodePtr child = xml->children;
    if(child == NULL) return;
    int n = _xmlChildElementCount(xml);
    
    xmlNodePtr* name_nodes = calloc(n, sizeof(xmlNodePtr));
    xmlNodePtr* lists = calloc(n, sizeof(xmlNodePtr));
    bool* empty = calloc(n, sizeof(bool));
    bool* multi = calloc(n, sizeof(bool));
    bool* optional = calloc(n, sizeof(bool));
    
		int len = 0;
    for(int i = 0; i < n; i++) {
      name_nodes[i] = child;
      if(child->children == NULL) {
        lists[i] = NULL;
      } else {
        lists[i] = child->children;
  			multi[i] = false;
        optional[i] = xmlGetProp(name_nodes[i], "optional") != NULL;
        if(lists[i] != NULL && !strcmp(lists[i]->name, "groups")) {
          lists[i] = lists[i]->children;
          multi[i] = true;
        }
  			if(lists[i] != NULL) {
  			  lists[i]->parent->extra = i;
  			  len += _xmlChildElementCount(lists[i]->parent);
			  }

        child->children = NULL;
      }
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
		xmlNodePtr* targets = calloc(sizeof(xmlNodePtr), n);
		
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
		
    free(targets);
    free(name_nodes);
    free(lists);
    free(optional);
    free(empty);
    free(multi);
    free(sortable);
    
    collate(groups);
  } else {
    xmlNodePtr child = xml->children;
    while(child != NULL){
      collate(child);
      child = child->next;
    }
  }
}

void
parsley_set_user_agent(char const * agent) {
  _parsley_set_user_agent(agent);
}

static void
unlink(xmlNodePtr parent, xmlNodePtr child) {
  if(child == NULL || parent == NULL) return;
  xmlNodePtr ptr = parent->children;
  
  if(ptr == child) {
    parent->children = child->next;
    if(child->next != NULL) {
      child->next->prev = NULL;
    }
  } else {
    while(ptr != NULL) {
      if(ptr->next == child) {
        ptr->next = child->next;
        if(child->next) child->next->prev = ptr;
      }
      ptr = ptr->next;
    }
  }  
  child->next = NULL;
  child->prev = NULL;
  child->parent = NULL;
}

static void
visit(parsedParsleyPtr ptr, xmlNodePtr xml, char* err);

static void 
prune(parsedParsleyPtr ptr, xmlNodePtr xml, char* err) {   
	if(xml == NULL || is_root(xml)) return;
  bool optional = xmlGetProp(xml, "optional") != NULL;
  if(optional) {
    xmlNodePtr parent = xml->parent;
    unlink(parent, xml);
    free(err);
    err = NULL;
    visit(ptr, parent, err);
    return;
  } else {
    if(err == NULL) asprintf(&err, "%s was empty", xpath_of(xml));
    if(!is_root(xml->parent)) {
      // fprintf(stderr, "prune up: %s\n", xml->parent->name);
      prune(ptr, xml->parent, err);
    } else {
      // fprintf(stderr, "error out\n");
      ptr->error = err;
    }
  }
}

static void
visit(parsedParsleyPtr ptr, xmlNodePtr xml, char* err) { 
  if(xml == NULL) return;
  // printf("trying to visit:      %s\n", xml->name);
  if(xml->type != XML_ELEMENT_NODE) return;
  xmlNodePtr child = xml->children;
  xmlNodePtr parent = xml->parent;
  if(parent == NULL) return;
  
  // printf("passed guard clause:      %s\n", xml->name);
  
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

static parsedParsleyPtr current_ptr = NULL;

void 
parsleyXsltError(void * ctx, const char * msg, ...) {
	if(current_ptr == NULL) return;
  va_list ap;
  va_start(ap, msg);
  if(current_ptr->error == NULL) {
    char *tmp;
    char *tmp2;
    vasprintf(&tmp, msg, ap);
    tmp2 = arepl(tmp, "xmlXPathCompOpEval: ", "");
    current_ptr->error = arepl(tmp2, "\n", "");
    
    free(tmp);
    free(tmp2);
  }
  va_end(ap);
}

static bool
hasDefaultNS(xmlDocPtr doc) {
	return xmlSearchNs(doc, doc->children, NULL) != NULL;
}

static void 
_killDefaultNS(xmlNodePtr node) {
	if(node == NULL) return;

	xmlNsPtr ns = node->nsDef;
	if(ns != NULL) {
		if(ns->prefix == NULL) node->nsDef = ns->next;
		xmlNsPtr prev = ns;
		while(ns = ns->next) {
			if(ns->prefix == NULL) prev->next = ns->next;
		}
	}
	
	ns = node->ns;
	if(ns != NULL) {
		if(ns->prefix == NULL) node->ns = ns->next;
		xmlNsPtr prev = ns;
		while(ns = ns->next) {
			if(ns->prefix == NULL) prev->next = ns->next;
		}
	}
	
	_killDefaultNS(node->children);
	_killDefaultNS(node->next);
}

void 
killDefaultNS(xmlDocPtr doc) {
	if(hasDefaultNS(doc)) {
		_killDefaultNS(doc->children);
	}
}

parsedParsleyPtr parsley_parse_doc(parsleyPtr parsley, xmlDocPtr doc, int flags) {
	killDefaultNS(doc);
	
  parsedParsleyPtr ptr = (parsedParsleyPtr) calloc(sizeof(parsed_parsley), 1);
  ptr->error = NULL;
  ptr->parsley = parsley;
  
  parsley_io_set_mode(flags);
  xsltTransformContextPtr ctxt = xsltNewTransformContext(parsley->stylesheet, doc);
  xmlSetGenericErrorFunc(ctxt, parsleyXsltError);
  current_ptr = ptr;
  
  if(flags & PARSLEY_OPTIONS_SGWRAP) { 
    doc = parsley_apply_span_wrap(doc);
  }
  ptr->xml = xsltApplyStylesheetUser(parsley->stylesheet, doc, NULL, NULL, NULL, ctxt);
  xsltFreeTransformContext(ctxt);
  current_ptr = NULL;
  
  if(ptr->error == NULL) {
    if(ptr->xml != NULL && ptr->error == NULL) {
      if(flags & PARSLEY_OPTIONS_COLLATE) collate(ptr->xml->children);
      if(flags & PARSLEY_OPTIONS_PRUNE) visit(ptr, ptr->xml->children, NULL);
    }
    if(ptr->xml == NULL && ptr->error == NULL) { // == NULL
      ptr->error = strdup("Internal runtime error");
    }
  }
	return ptr;
}

static bool 
json_invalid_object(parsleyPtr ptr, struct json_object *json) {
	json_object_object_foreach(json, key, val) {
		if(val==NULL) ptr->error = strdup("Parselets can only be made up of strings, arrays, and objects.");
		
		switch(json_object_get_type(val)) {
		case json_type_string:
			break;
		case json_type_array:
			if(json_object_array_length(val) != 1) {
				ptr->error = strdup("Parselet arrays should have length 1.");
				return true;
			}
			struct json_object * inner = json_object_array_get_idx(val, 0);
			switch(json_object_get_type(inner)) {
			case json_type_string:
				break;
			case json_type_object:
				if(json_invalid_object(ptr, inner)) return true;
				break;
			default:
				ptr->error = strdup("Arrays may contain either a single string or an object.");
				return true;
			}
			break;
		case json_type_object:
			if(json_invalid_object(ptr, val)) {
				return true;
			}
			break;
		default:
			ptr->error = strdup("Parselets can only be made up of strings, arrays, and objects.");
		}
    if(val == NULL || !json_object_is_type(val, json_type_string)) return false;
  }
	return false;
}

static bool 
json_invalid(parsleyPtr ptr, struct json_object *json) {
	if(!json_object_is_type(json, json_type_object)) {
		ptr->error = strdup("The parselet root must be an object");
		return true;
	}
	return json_invalid_object(ptr, json);
}

static void free_context(contextPtr c) {
  if(c == NULL) return;
  if(c->tag != NULL) free(c->tag);
  if(c->filter != NULL) pxpath_free(c->filter);
  if(c->expr != NULL) pxpath_free(c->expr);
  
  if(c->parent != NULL && c->parent->child != NULL) {
    if(c->parent->child == c) {
      c->parent->child = NULL;
    } else {
      contextPtr ptr = c->parent->child;
      while(ptr->next != NULL) {
        if(ptr->next == c) {
          ptr->next = NULL;
        } else {
          ptr = ptr->next;
        }
      }
    }
  }
  if(c->next != NULL) free_context(c->next);
  if(c->child != NULL) free_context(c->child);
  free(c);
}

static contextPtr 
new_context(struct json_object * json, xmlNodePtr node) {
	contextPtr c = calloc(sizeof(parsley_context), 1);
	c->node = node;
	c->ns = node->ns;
  c->tag = strdup("root");
	c->expr = pxpath_new_path(1, "/");
	c->json = json;
	return c;
}

parsleyPtr parsley_compile(char* parsley_str, char* incl) {
	parsleyPtr parsley = (parsleyPtr) calloc(sizeof(compiled_parsley), 1);
	
	if(last_parsley_error != NULL) {
		free(last_parsley_error);
		last_parsley_error = NULL;
	}
	
  registerEXSLT();
	
  // struct json_tokener *tok = json_tokener_new();
  //  struct json_object *json = json_tokener_parse_ex(tok, parsley_str);
  //  
	struct json_tokener *tok = json_tokener_new();
  struct json_object *json = json_tokener_parse_ex(tok, parsley_str, -1);
  int error_offset = tok->char_offset;
  if(tok->err != json_tokener_success)
    json = error_ptr(-tok->err);
  json_tokener_free(tok);
  
	if(is_error(json)) {
		asprintf(&(parsley->error), "Your parselet is not valid json: %s at char:%d", json_tokener_errors[-(unsigned long) json], error_offset);
		return parsley;
	}
	
	if(json_invalid(parsley, json)) {
		// fprintf(stderr, "Invalid parselet structure: %s\n", parsley->error);
		return parsley;
	}

	xmlNodePtr node = new_stylesheet_skeleton(incl);
		
	contextPtr context = new_context(json, node);
	__parsley_recurse(context);
	
	json_object_put(json); // frees json
	parsley->error = last_parsley_error;
	
	if(parsley->error == NULL) {
		parsley->stylesheet = xsltParseStylesheetDoc(node->doc);
	}
	
  free_context(context);
	return parsley;
}

contextPtr deeper_context(contextPtr context, char* key, struct json_object * val) {
	contextPtr c = (contextPtr) calloc(sizeof(parsley_context), 1);
	c->node = context->node;
	c->ns = context->ns;
	c->parent = context;
	c->tag = parsley_key_tag(key);
  c->flags = parsley_key_flags(key);
	parsley_parsing_context = c;
	c->array = val != NULL && json_object_is_type(val, json_type_array);
	c->json = c->array ? json_object_array_get_idx(val, 0) : val;
	c->string = val != NULL && json_object_is_type(c->json, json_type_string);
	c->filter = parsley_key_filter(key);
	c->magic = context->array && context->filter == NULL;
	c->expr = c->string ? myparse(json_object_get_string(c->json)) : NULL;
	if(context->child == NULL) {
		context->child = c;
	} else {
		contextPtr tmp = context->child;
		while(tmp->next != NULL) tmp = tmp->next;
		tmp->next = c;
	}
  // printf(stderr, "json:    %s\ntag:    %s\nexpr:   %s\nfilter: %s\n\n", json_object_get_string(c->json), c->tag, pxpath_to_string(c->expr), pxpath_to_string(c->filter));
	return c;
}

void parsley_free(parsleyPtr ptr) {
	if(ptr->error != NULL) 						
			free(ptr->error);
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

static bool 
all_strings(struct json_object * json) {  
	json_object_object_foreach(json, key, val) {
    if(val == NULL || !json_object_is_type(val, json_type_string)) return false;
  }
  return true;
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
  return c->filter == NULL && c->expr != NULL && inner_path(c->expr) != NULL;
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

	// printf("node %s\n", c->node->name);
	xmlNsPtr parsley = c->ns;
	xmlNsPtr xsl = xmlDocGetRootElement(c->node->doc)->ns;
  
	if(c->array)                c->node = xmlNewChild(c->node, parsley, "groups", NULL);
  if(filtered) {
															c->node = xmlNewChild(c->node, xsl, "for-each", NULL);
															xmlSetProp(c->node, "select", filter);
	}
  if(filtered && !multiple) {
															c->node = xmlNewChild(c->node, xsl, "if", NULL);
															xmlSetProp(c->node, "test", "position() = 1");
  }
  if(multiple) {
	               							c->node = xmlNewChild(c->node, parsley, "group", NULL);
															if (!(c->flags & PARSLEY_BANG)) xmlSetProp(c->node, "optional", "true");
	}
	xmlNodePtr attr = xmlNewChild(c->node, xsl, "attribute", NULL);
	xmlSetProp(attr, "name", "position");
	xmlNodePtr counter = xmlNewChild(attr, xsl, "value-of", NULL);
	xmlSetProp(counter, "select", "count(preceding::*) + count(ancestor::*)");
  
  if(c->string) {
		c->node = xmlNewChild(c->node, xsl, "value-of", NULL);
		xmlSetProp(c->node, "select", expr);
  } else {
    if(magic_children)        c->node = xmlNewChild(c->node, parsley, "zipped", NULL);
    __parsley_recurse(c);
  }
  
  if(filter !=NULL) free(filter);
  if(expr !=NULL)   free(expr);
}

void __parsley_recurse(contextPtr context) {
	contextPtr c;
	if(context->json == NULL) return;
	// printf(stderr, "<%s> %s\n", context->tag, context->node->name);
	json_object_object_foreach(context->json, key, val) {
		c = deeper_context(context, key, val);
		// printf(stderr, "<%s>\n", c->tag);
		c->node = xmlAddChild(c->node, xmlNewNode(NULL, c->tag));
		if (c->flags & PARSLEY_OPTIONAL) xmlSetProp(c->node, "optional", "true");
    render(c);
	}  
}


// static char* full_expr(contextPtr context, char* expr) {
// 	if(expr == NULL) return context->full_expr;
// 	char* merged = arepl(expr, ".", context->full_expr);
// 	return arepl(merged, "///", "//");
// }
static char* 
inner_key_each(struct json_object * json);

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

static char* 
inner_key_each(struct json_object * json) {
	json_object_object_foreach(json, key, val) {
		char* inner = inner_key_of(val);
		if(inner != NULL) return inner;
	}
	return NULL;
}
