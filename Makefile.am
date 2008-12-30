AM_YFLAGS = -d
BUILT_SOURCES=parser.h

lib_LTLIBRARIES = libdexter.la
libdexter_la_SOURCES = xml2json.c regexp.c printbuf.c util.c kstring.c obstack.c scanner.l parser.y dexter.c
include_HEADERS = dexter.h obstack.h xml2json.h

bin_PROGRAMS = dexterc dexter

dexterc_SOURCES = dexterc_main.c
dexterc_LDADD = libdexter.la

dexter_SOURCES = dexter_main.c
dexter_LDADD = libdexter.la
