#include "ruby.h"
#include <dexter.h>

VALUE dexCompile(VALUE, VALUE, VALUE);
static char* dexError;

void Init_dexterous()
{
  VALUE dexterous = rb_define_module("Dexterous");
	rb_define_singleton_method(dexterous, "compile", dexCompile, 2);
}

VALUE dexCompile(VALUE self, VALUE dex, VALUE incl){
	VALUE out;
	char* cdex = STR2CSTR(dex);
	char* cincl = STR2CSTR(incl);
	char* cout = dex_compile(cdex, cincl);
	if(cout !=NULL) {
		out = rb_str_new2(cout);
		free(cout);
	} else{
		out = rb_str_new2(dexError);
	}
	return out;
}

void dex_error(char* message) {
	dexError = message;
}