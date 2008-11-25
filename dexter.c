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

char* dex_set_debug_mode(int i) {
	dex_debug_mode = i;
}

char* dex_compile(char* dex, char* incl) {
	dex_error_state = 0;
	obstack_init(&dex_obstack);
	
	struct json_object *json = json_tokener_parse(dex);
	if(is_error(json)) {
		dex_error("Your dex is not valid json.");
		return NULL;
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
	sprintbuf(buf, " exclude-result-prefixes=\"str math set func dyn exsl saxon user date\"");
	sprintbuf(buf, ">\n");
	sprintbuf(buf, "<xsl:output method=\"xml\" indent=\"yes\"/>\n");
	sprintbuf(buf, "<xsl:strip-space elements=\"*\"/>\n");
	sprintbuf(buf, "%s\n", incl);
	sprintbuf(buf, "<xsl:template match=\"/\">\n");
	sprintbuf(buf, "<root>\n");
	
	char *context = "root";
	__dex_recurse(json, buf, context);
	json_object_put(json); // frees json
	
	sprintbuf(buf, "</root>\n");
	sprintbuf(buf, "</xsl:template>\n");
	sprintbuf(buf, "</xsl:stylesheet>\n");
	
	char* output = strdup(buf->buf);
	printbuf_free(buf);
	obstack_free(&dex_obstack, NULL);
	
	if(dex_error_state != 0) return NULL;
	return output;
}


void * __dex_alloc(int size) {
	return obstack_alloc(&dex_obstack, size);
}

void __dex_recurse_object(struct json_object * json, struct printbuf* buf, char *context) {
	char *tag;
	char *ptr;
	char *expr;
	int offset;
	bool has_expr;
	json_object_object_foreach(json, key, val) {
		offset = 0;
		tag = astrdup(key);
		expr = astrdup(key);
		ptr = tag;
		has_expr = false;
		while(*ptr++ != '\0'){
			offset++;
			if(*ptr == '(') {
				*ptr = 0;
				has_expr = true;
				break;
			}
		}
		expr += offset;
		
		sprintbuf(buf, "<%s>\n", tag);
		if(has_expr) sprintbuf(buf, "<xsl:for-each select=\"%s\">\n", myparse(expr));
		__dex_recurse(val, buf, astrcat3(context, ".", tag));
		if(has_expr) sprintbuf(buf, "</xsl:for-each>\n");
		sprintbuf(buf, "</%s>\n", tag);
  }
}

void __dex_recurse_array(struct json_object * json, struct printbuf* buf, char *context) {
	for(int i = 0; i < json_object_array_length(json); i++) {
		sprintbuf(buf, "<dexter:group>\n");
 		__dex_recurse(json_object_array_get_idx(json, i), buf, context);
    sprintbuf(buf, "</dexter:group>\n");
  }
}

void __dex_recurse_string(struct json_object * json, struct printbuf* buf, char *context) {
	char* a = astrdup(json_object_get_string(json));
	char* ptr = context;
	char* last = context;
	char* expr;
	while(*ptr++){
		if(*ptr == '.') last = ptr + 1;
	}
	dex_parsing_context = context;
	expr = myparse(a);
	sprintbuf(buf, "<xsl:variable name=\"%s\" select=\"%s\" />\n", context, expr);
	sprintbuf(buf, "<xsl:variable name=\"%s\" select=\"$%s\" />\n", last, context);
	sprintbuf(buf, "<xsl:value-of select=\"$%s\" />\n", context);
}

void yyerror(const char * s) {
	struct printbuf *buf = printbuf_new();
  sprintbuf(buf, "%s in key: %s", s, dex_parsing_context);
	
	dex_error_state = 1;
	dex_error(buf->buf);
	printbuf_free(buf);
}

void __dex_recurse(struct json_object * json, struct printbuf* buf, char *context) {
	switch(json_object_get_type(json)){
		case json_type_object:
			__dex_recurse_object(json, buf, context);
			break;
		case json_type_array:
			__dex_recurse_array(json, buf, context);
			break;
		case json_type_string:
			__dex_recurse_string(json, buf, context);
			break;
		case json_type_boolean:
		case json_type_double:
		case json_type_int:
		default:
			fprintf(stderr, "Invalid type in json\n");
			exit(1);
	}
}
