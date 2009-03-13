SUBDIRS=json-c-0.8 .
AM_YFLAGS = -d
BUILT_SOURCES=parser.h

lib_LTLIBRARIES = libparsley.la
libparsley_la_SOURCES = parsed_xpath.c xml2json.c regexp.c functions.c util.c scanner.l parser.y parsley.c
include_HEADERS = parsley.h xml2json.h json-c-0.8/printbuf.h

bin_PROGRAMS = parsleyc parsley

parsleyc_SOURCES = parsleyc_main.c
parsleyc_LDADD = libparsley.la json-c-0.8/libjson.la

parsley_SOURCES = parsley_main.c
parsley_LDADD = libparsley.la json-c-0.8/libjson.la

bisect:
	./bootstrap.sh && ./configure && make clean && make check

valgrind: parsley
	libtool --mode=execute valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes ./parsley test/fictional.let test/fictional.html

install-all:
	./bootstrap.sh && ./configure && make && make install && cd ruby && rake install && cd ../python && python setup.py install

check-am:
	@echo "fictional..."; ./parsley test/fictional.let test/fictional.html 2>&1 | diff test/fictional.json - && echo "    success."
	@echo "fictional-opt..."; ./parsley test/fictional-opt.let test/fictional-opt.html 2>&1 | diff test/fictional-opt.json - && echo "    success."
	@echo "function-magic..."; ./parsley test/function-magic.let test/function-magic.html 2>&1 | diff test/function-magic.json - && echo "    success."
	@echo "malformed-expr..."; ./parsley test/malformed-expr.let test/malformed-expr.html 2>&1 | diff test/malformed-expr.json - && echo "    success."
	@echo "malformed-json..."; ./parsley test/malformed-json.let test/malformed-json.html 2>&1 | diff test/malformed-json.json - && echo "    success."
	@echo "css_attr..."; ./parsley -x test/css_attr.let test/css_attr.html 2>&1 | diff test/css_attr.json - && echo "    success."
	@echo "match..."; ./parsley -x test/match.let test/match.xml 2>&1 | diff test/match.json - && echo "    success."
	@echo "position..."; ./parsley test/position.let test/position.html 2>&1 | diff test/position.json - && echo "    success."
	@echo "replace..."; ./parsley -x test/replace.let test/replace.xml 2>&1 | diff test/replace.json - && echo "    success."
	@echo "scope..."; ./parsley test/scope.let test/scope.html 2>&1 | diff test/scope.json - && echo "    success."
	@echo "test..."; ./parsley -x test/test.let test/test.xml 2>&1 | diff test/test.json - && echo "    success."
	@echo "yelp..."; ./parsley test/yelp.let test/yelp.html 2>&1 | diff test/yelp.json - && echo "    success."
	@echo "optional..."; ./parsley test/optional.let test/optional.html 2>&1 | diff test/optional.json - && echo "    success."
	@echo "empty..."; ./parsley test/empty.let test/empty.html 2>&1 | diff test/empty.json - && echo "    success."
	@echo "trivial..."; ./parsley test/trivial.let test/trivial.html 2>&1 | diff test/trivial.json - && echo "    success."
	@echo "trivial2..."; ./parsley test/trivial2.let test/trivial2.html 2>&1 | diff test/trivial2.json - && echo "    success."
	@echo "craigs-simple..."; ./parsley test/craigs-simple.let test/craigs-simple.html 2>&1 | diff test/craigs-simple.json - && echo "    success."
	@echo "yelp-home..."; ./parsley test/yelp-home.let test/yelp-home.html 2>&1 | diff test/yelp-home.json - && echo "    success."
	@echo "cool..."; perl -e 'alarm shift @ARGV; exec @ARGV' 1 ./parsley test/cool.let test/cool.html 2>&1 | diff test/cool.json - && echo "    success."
	@echo "bang..."; ./parsley test/bang.let test/bang.html 2>&1 | diff test/bang.json - && echo "    success."
	@echo "unbang..."; ./parsley test/unbang.let test/unbang.html 2>&1 | diff test/unbang.json - && echo "    success."
