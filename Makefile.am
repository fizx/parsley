AM_YFLAGS = -d
BUILT_SOURCES=parser.h

lib_LTLIBRARIES = libparsley.la
libparsley_la_SOURCES = parsed_xpath.c xml2json.c regexp.c functions.c util.c scanner.l parser.y parsley.c
include_HEADERS = parsley.h xml2json.h parsed_xpath.h

bin_PROGRAMS = parsleyc parsley

parsleyc_SOURCES = parsleyc_main.c
parsleyc_LDADD = libparsley.la

parsley_SOURCES = parsley_main.c
parsley_LDADD = libparsley.la

valgrind:
	./libtool --mode=execute valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes ./parsley test/fictional.let test/fictional.html

profile:
	./libtool --mode=execute valgrind --tool=callgrind ./parsley test/fictional.let test/fictional.html

install-all:
	./bootstrap.sh && ./configure && make && make install && cd ruby && rake install && cd ../python && python setup.py install
	
bench:
	@echo "yelp..."; ./parsley test/yelp.let test/yelp.html > /dev/null
	@echo "craigs-simple..."; ./parsley test/craigs-simple.let test/craigs-simple.html > /dev/null
	@echo "yelp-home..."; ./parsley test/yelp-home.let test/yelp-home.html > /dev/null
	@echo "craigs..."; ./parsley test/craigs.let test/craigs.html > /dev/null

check-am:
	@echo "ambiguous..."; ./parsley test/ambiguous.let test/ambiguous.html 2>&1 | diff test/ambiguous.json - && echo "    success."
	@echo "unicode..."; ./parsley test/unicode.let test/unicode.html 2>&1 | diff test/unicode.json - && echo "    success."
	@echo "contains..."; ./parsley test/contains.let test/contains.html 2>&1 | diff test/contains.json - && echo "    success."
	@echo "math_ambiguity..."; ./parsley test/math_ambiguity.let test/math_ambiguity.html 2>&1 | diff test/math_ambiguity.json - && echo "    success."
	@echo "content..."; ./parsley test/content.let test/content.html 2>&1 | diff test/content.json - && echo "    success."
	@echo "whitespace..."; ./parsley test/whitespace.let test/whitespace.html 2>&1 | diff test/whitespace.json - && echo "    success."
	@echo "backslash..."; ./parsley test/backslash.let test/backslash.html 2>&1 | diff test/backslash.json - && echo "    success."
	@echo "question_regressions..."; ./parsley test/question_regressions.let test/question_regressions.html 2>&1 | diff test/question_regressions.json - && echo "    success."
	@echo "outer-xml..."; ./parsley test/outer-xml.let test/outer-xml.html 2>&1 | diff test/outer-xml.json - && echo "    success."
	@echo "segfault..."; ./parsley test/segfault.let test/segfault.html 2>&1 | diff test/segfault.json - && echo "    success."
	@echo "sg_off..."; ./parsley test/sg_off.let test/sg_off.html 2>&1 | diff test/sg_off.json - && echo "    success."
	@echo "nth-regression..."; ./parsley test/nth-regression.let test/nth-regression.html 2>&1 | diff test/nth-regression.json - && echo "    success."
	@echo "array-regression..."; ./parsley test/array-regression.let test/array-regression.html 2>&1 | diff test/array-regression.json - && echo "    success."
	@echo "fictional..."; ./parsley test/fictional.let test/fictional.html 2>&1 | diff test/fictional.json - && echo "    success."
	@echo "fictional-opt..."; ./parsley test/fictional-opt.let test/fictional-opt.html 2>&1 | diff test/fictional-opt.json - && echo "    success."
	@echo "function-magic..."; ./parsley test/function-magic.let test/function-magic.html 2>&1 | diff test/function-magic.json - && echo "    success."
	@echo "malformed-expr..."; ./parsley test/malformed-expr.let test/malformed-expr.html 2>&1 | diff test/malformed-expr.json - && echo "    success."
	@echo "malformed-json..."; ./parsley test/malformed-json.let test/malformed-json.html 2>&1 | diff test/malformed-json.json - && echo "    success."
	@echo "malformed-array..."; ./parsley test/malformed-array.let test/malformed-array.html 2>&1 | diff test/malformed-array.json - && echo "    success."
	@echo "css_attr..."; ./parsley -x test/css_attr.let test/css_attr.html 2>&1 | diff test/css_attr.json - && echo "    success."
	@echo "position..."; ./parsley test/position.let test/position.html 2>&1 | diff test/position.json - && echo "    success."
	@echo "replace..."; ./parsley -x test/replace.let test/replace.xml 2>&1 | diff test/replace.json - && echo "    success."
	@echo "scope..."; ./parsley test/scope.let test/scope.html 2>&1 | diff test/scope.json - && echo "    success."
	@echo "test..."; ./parsley -x test/test.let test/test.xml 2>&1 | diff test/test.json - && echo "    success."
	@echo "yelp..."; ./parsley test/yelp.let test/yelp.html 2>&1 | diff test/yelp.json - && echo "    success."
	@echo "optional..."; ./parsley test/optional.let test/optional.html 2>&1 | diff test/optional.json - && echo "    success."
	@echo "empty..."; ./parsley test/empty.let test/empty.html 2>&1 | diff test/empty.json - && echo "    success."
	@echo "craigs-simple..."; ./parsley test/craigs-simple.let test/craigs-simple.html 2>&1 | diff test/craigs-simple.json - && echo "    success."
	@echo "yelp-home..."; ./parsley test/yelp-home.let test/yelp-home.html 2>&1 | diff test/yelp-home.json - && echo "    success."
	@echo "bang..."; ./parsley test/bang.let test/bang.html 2>&1 | diff test/bang.json - && echo "    success."
	@echo "cool..."; perl -e 'alarm shift @ARGV; exec @ARGV' 1 ./parsley test/cool.let test/cool.html 2>&1 | diff test/cool.json - && echo "    success."
	@echo "trivial2..."; ./parsley test/trivial2.let test/trivial2.html 2>&1 | diff test/trivial2.json - && echo "    success."
	@echo "trivial..."; ./parsley test/trivial.let test/trivial.html 2>&1 | diff test/trivial.json - && echo "    success."
	@echo "match..."; ./parsley -x test/match.let test/match.xml 2>&1 | diff test/match.json - && echo "    success."
	@echo "quote..."; ./parsley test/quote.let test/quote.xml 2>&1 | diff test/quote.json - && echo "    success."
	@echo "unbang..."; ./parsley test/unbang.let test/unbang.html 2>&1 | diff test/unbang.json - && echo "    success."
	@echo "malformed-xpath..."; ./parsley test/malformed-xpath.let test/malformed-xpath.html 2>&1 | diff test/malformed-xpath.json - && echo "    success."
	@echo "remote-fail..."; ./parsley -z test/remote.let test/remote.html 2>&1 | diff test/remote-fail.json - && echo "    success."
	@echo "crash..."; ./parsley test/crash.let test/crash.html 2>&1 | diff test/crash.json - && echo "    success."
	@echo "reddit..."; ./parsley test/reddit.let test/reddit.html 2>&1 | diff test/reddit.json - && echo "    success."
	@echo "div..."; ./parsley test/div.let test/div.html 2>&1 | diff test/div.json - && echo "    success."
	@echo "default-namespace..."; ./parsley -x test/default-namespace.let test/default-namespace.xml 2>&1 | diff test/default-namespace.json - && echo "    success."
	@echo "sg-wrap..."; ./parsley -s test/sg-wrap.let test/sg-wrap.html 2>&1 | diff test/sg-wrap.json - && echo "    success."
	@echo "collate_regression..."; ./parsley test/collate_regression.let test/collate_regression.html 2>&1 | diff test/collate_regression.json - && echo "    success."
	