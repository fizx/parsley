#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/debugXML.h>
#include <json/json.h>
#include "xml2json.h"
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

static struct json_object * _xml2json(xmlNodePtr xml) {
  if(xml == NULL) return NULL;
  
  xmlNodePtr child;
  struct json_object * json = json_object_new_string("bogus");
  
  switch(xml->type) {
    case XML_ELEMENT_NODE:
      child = xml->children;
      if(xml->ns == NULL) {
        child = xml;
        json = json_object_new_object();
        while(child != NULL) {
          json_object_object_add(json, child->name, xml2json(child->children));
          child = child->next;
        }
      } else if(!strcmp(xml->ns->prefix, "dexter")) {
        if(!strcmp(xml->name, "zipped")) {
          int len = 0;
          xmlNodePtr ptr = xml->children;
          while(ptr != NULL){
            len++;
            ptr = ptr->next;
          }
          xmlNodePtr *ptrs = (xmlNodePtr *) malloc(len * sizeof(xmlNodePtr));
          char **names = (char**) malloc(len * sizeof(char*));
          for(int i = 0; i < len; i++) {
            //    zip named  groups    
            ptrs[i] = child->children->children;
            names[i] = child->name;
            child = child->next;
          }
          json = json_object_new_array();
          bool legit = true;
          while(legit) {
            for(int i = 0; i < len; i++) {
              // fprintf(stderr, "name: %s\n", names[i]);
              // xmlDebugDumpNode(stderr, ptrs[i], 2);
              // exit(1);
              legit &= ptrs[i] != NULL;
            }
            if(legit) {
              struct json_object * inner = json_object_new_object();
              for(int i = 0; i < len; i++) {
                json_object_object_add(inner, names[i], xml2json(ptrs[i]->children));
                ptrs[i] = ptrs[i]->next;
              }
              json_object_array_add(json, inner);
            }
          }
          free(ptrs);
          free(names);
        } else if(!strcmp(xml->name, "groups")) {
          json = json_object_new_array();          
          while(child != NULL) {
            json_object_array_add(json, xml2json(child->children));
            child = child->next;
          }          
        } else if(!strcmp(xml->name, "group")) {
          // Implicitly handled by dexter:groups handler
        }
      }
      break;
    case XML_TEXT_NODE:
      json = json_object_new_string(xml->content);
      break;
  }
  return json;
}

/**
 * Handles a simplified xml
 */
struct json_object * xml2json(xmlNodePtr xml) {
	struct json_object * json = _xml2json(xml);
	if(json == NULL) json = json_object_new_object();
	return json;
}
