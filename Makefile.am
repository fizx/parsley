lib_LTLIBRARIES = libdexter.la
libdexter_la_SOURCES = printbuf.c kstring.c obstack.c scanner.l parser.y dexter.c

include_HEADERS = dexter.h obstack.h

bin_PROGRAMS = dexterc
dexterc_SOURCES = dexterc_main.c
dexterc_LDADD = @LEXLIB@ libdexter.la
