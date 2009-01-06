#include "dexter.h"
#include "vexter.h"
#include "printbuf.h"
#include "dex_mem.h"
#include "util.h"
#include "kstring.h"

static void vex_recurse(struct json_object * json, struct printbuf * buf, char* path) {
	json_object_object_foreach(json, key, val) {
    sprintbuf(buf, "<xsl:template match=\"%s\">", astrcat3(path, "/", key));
    sprintbuf(buf, "</xsl:template>"); 
  }
}

dexPtr vex_compile(char* vex_str, char* incl){
  if(incl == NULL) incl = "";
  printf("yay: %s\n", astrcat("a", "b"));
  dexPtr vex = (dexPtr) calloc(sizeof(compiled_dex), 1);
  struct json_object *json = json_tokener_parse(vex_str);
	if(is_error(json)) {
		vex->error = strdup("Your vex is not valid json.");
		return vex;
	}
	
  struct printbuf* buf = printbuf_new();
  sprintbuf_dex_header(buf);
	sprintbuf(buf, incl);
	vex_recurse(json, buf, "/");
	sprintbuf(buf, "</xsl:stylesheet>\n");
	
	if(vex->error == NULL) {
		xmlParserCtxtPtr ctxt = xmlNewParserCtxt();
		xmlDocPtr doc = xmlCtxtReadMemory(ctxt, buf->buf, buf->size, "http://kylemaxwell.com/vexter/compiled", NULL, 3);
		vex->raw_stylesheet = strdup(buf->buf);
		vex->stylesheet = xsltParseStylesheetDoc(doc);
	}
	
	printbuf_free(buf);
	dex_collect();
	
  return vex;
}

xmlDocPtr vex_parse_doc(dexPtr vex, xmlDocPtr input) {
  return input;
}