require 'mkmf'
$CFLAGS += " -ldexter "
have_header("../src/dexter.h")
have_library("dexter", "dex_compile", ".");
create_makefile("dexter")