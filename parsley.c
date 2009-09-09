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
	if(html) {
		htmlParserCtxtPtr htmlCtxt = htmlNewParserCtxt();
  	htmlDocPtr html = htmlCtxtReadFile(htmlCtxt, file, NULL, HTML_PARSE_RECOVER | HTML_PARSE_NOERROR |HTML_PARSE_NOWARNING);
    htmlFreeParserCtxt(htmlCtxt);
    if(html == NULL) return parse_error("Couldn't parse file: %s\n", file);
    parsedParsleyPtr out = parsley_parse_doc(parsley, html, flags);
    xmlFreeDoc(html);
    return out;
	} else {
		xmlParserCtxtPtr ctxt = xmlNewParserCtxt();
		xmlDocPtr xml = xmlCtxtReadFile(ctxt, file, NULL, HTML_PARSE_RECOVER | HTML_PARSE_NOERROR |HTML_PARSE_NOWARNING);
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
	if(base_uri == NULL) base_uri = "http://parselets.com/in-memory-string";
	if(html) {
		htmlParserCtxtPtr htmlCtxt = htmlNewParserCtxt();
  	htmlDocPtr html = htmlCtxtReadMemory(htmlCtxt, string, size, base_uri, NULL, HTML_PARSE_RECOVER | HTML_PARSE_NOERROR |HTML_PARSE_NOWARNING);
    if(html == NULL) return parse_error("Couldn't parse string");
    parsedParsleyPtr out = parsley_parse_doc(parsley, html, flags);
    xmlFreeDoc(html);
    return out;
	} else {
		xmlParserCtxtPtr ctxt = xmlNewParserCtxt();
 		xmlDocPtr xml = xmlCtxtReadMemory(ctxt, string, size, base_uri, NULL, HTML_PARSE_RECOVER | HTML_PARSE_NOERROR |HTML_PARSE_NOWARNING);
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
visit(parsedParsleyPtr ptr, xmlNod