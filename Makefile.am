AM_YFLAGS = -d
BUILT_SOURCES=parser.h

lib_LTLIBRARIES = libparsley.la
libparsley_la_SOURCES = parsley_mem.c xml2json.c regexp.c printbuf.c functions.c util.c kstring.c obstack.c scanner.l parser.y parsley.c
include_HEADERS = parsley.h obstack.h xml2json.h

bin_PROGRAMS = parsleyc parsley

parsleyc_SOURCES = parsleyc_main.c
parsleyc_LDADD = libparsley.la

parsley_SOURCES = parsley_main.c
parsley_LDADD = libparsley.la

bisect:
	./bootstrap.sh && ./configure && make clean && make check

install-all:
	./bootstrap.sh && ./configure && make && make install && cd ruby && rake install && cd ../python && python setup.py install

check-am:
	@echo "fictional..."; ./parsley test/fictional.let test/fictional.html | diff test/fictional.json - && echo "    success."
	@echo "fictional-opt..."; ./parsley test/fictional-opt.let test/fictional-opt.html | diff test/fictional-opt.json - && echo "    success."
	@echo "function-magic..."; ./parsley test/function-magic.let test/function-magic.html | diff test/function-magic.json - && echo "    success."
	@echo "malformed-expr..."; ./parsley test/malformed-expr.let test/malformed-expr.html | diff test/malformed-expr.json - && echo "    success."
	@echo "malformed-json..."; ./parsley test/malformed-json.let test/malformed-json.html | diff test/malformed-json.json - && echo "    success."
	@echo "css_attr..."; ./parsley -x test/css_attr.let test/css_attr.html | diff test/css_attr.json - && echo "    success."
	@echo "match..."; ./parsley -x test/match.let test/match.xml | diff test/match.json - && echo "    success."
	@echo "position..."; ./parsley test/position.let test/position.html | diff test/position.json - && echo "    success."
	@echo "replace..."; ./parsley -x test/replace.let test/replace.xml | diff test/replace.json - && echo "    success."
	@echo "scope..."; ./parsley test/scope.let test/scope.html | diff test/scope.json - && echo "    success."
	@echo "test..."; ./parsley -x test/test.let test/test.xml | diff test/test.json - && echo "    success."
	@echo "yelp..."; ./parsley test/yelp.let test/yelp.html | diff test/yelp.json - && echo "    success."
	@echo "optional..."; ./parsley test/optional.let test/optional.html | diff test/optional.json - && echo "    success."
	@echo "malformed-function..."; ./parsley test/malformed-function.let test/malformed-function.html | diff test/malformed-function.json - && echo "    success."
	@echo "empty..."; ./parsley test/empty.let test/empty.html | diff test/empty.json - && echo "    success."
	@echo "trivial..."; ./parsley test/trivial.let test/trivial.html | diff test/trivial.json - && echo "    success."
	@echo "trivial2..."; ./parsley test/trivial2.let test/trivial2.html | diff test/trivial2.json - && echo "    success."
	@echo "craigs-simple..."; ./parsley test/craigs-simple.let test/craigs-simple.html | diff test/craigs-simple.json - && echo "    success."
	@echo "yelp-home..."; ./parsley test/yelp-home.let test/yelp-home.html | diff test/yelp-home.json - && echo "    success."