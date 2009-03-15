#ifndef XML2JSON_H_INCLUDED
#define XML2JSON_H_INCLUDED

#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/debugXML.h>
#include <json/json.h>

struct json_object * xml2json(xmlNodePtr);

#endif