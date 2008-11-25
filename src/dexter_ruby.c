#include "ruby.h"
#include "dexter.h"

VALUE cDexter;

static VALUE t_parse(VALUE dex, VALUE incl)
{
	ID to_s = rb_intern("to_s");
  VALUE arr;

	char* cstr = dex_compile(STR2CSTR(rb_funcall(dex, to_s, 0)), STR2CSTR(rb_funcall(incl, to_s, 0)));
	VALUE out = rb_str_new2(cstr);
	free(cstr);

  return out;
}

void Init_dexter() {
	cDexter = rb_define_module("Dexter");
	rb_define_module_function(cDexter, "parse", t_parse, 2);
}