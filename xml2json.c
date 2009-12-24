#include "xml2json.h"

static struct json_object * _xml2json(xmlNodePtr xml) {
  if(xml == NULL) return NULL;
  
  xmlNodePtr child;
  struct json_object * json = NULL;
  
  switch(xml->type) {
    case XML_ELEMENT_NODE:
      child = xml->children;
      if(xml->ns == NULL) {
        child = xml;
        // json_object_put(json);
        json = json_object_new_object();
        while(child != NULL) {
          json_object_object_add(json, child->name, xml2json(child->children));
          child = child->next;
        }
      } else if(!strcmp(xml->ns->prefix, "parsley")) {
        if(!strcmp(xml->name, "groups")) {
          // json_object_put(json);
          json = json_object_new_array();          
          while(child != NULL) {
            json_object_array_add(json, xml2json(child->children));
            child = child->next;
          }          
        } else if(!strcmp(xml->name, "group")) {
          // Implicitly handled by parsley:groups handler
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
