AM_YFLAGS = -d
BUILT_SOURCES=parser.h

lib_LTLIBRARIES = libdexter.la
libdexter_la_SOURCES = xml2json.c regexp.c printbuf.c functions.c util.c kstring.c obstack.c scanner.l parser.y dexter.c
include_HEADERS = dexter.h obstack.h xml2json.h

bin_PROGRAMS = dexterc dexter

dexterc_SOURCES = dexterc_main.c
dexterc_LDADD = libdexter.la

dexter_SOURCES = dexter_main.c
dexter_LDADD = libdexter.la

port:
	make clean
	rm -rf /tmp/dexter-`cat VERSION`
	cp -R . /tmp/dexter-`cat VERSION`
	tar -C /tmp/  --exclude release --exclude .git -zcf "/tmp/dexter-`cat VERSION`.tar.gz" dexter-`cat VERSION`
	rsync --progress "/tmp/dexter-`cat VERSION`.tar.gz" kylemaxwell.com:/var/www/kylemaxwell_com/dexter/
	cat Portfile.in | sed "s/<VERSION>/`cat VERSION`/" > Portfile
	echo "checksums \
					md5 `md5 /tmp/dexter-\`cat VERSION\`.tar.gz | sed "s/.*= //"` \
					sha1 `openssl sha1 /tmp/dexter-\`cat VERSION\`.tar.gz | sed "s/.*= //"` \
					rmd160 `openssl rmd160 /tmp/dexter-\`cat VERSION\`.tar.gz | sed "s/.*= //"`" \
					>> Portfile
	sudo port build

check-am:
	@echo "fictional..."; ./dexter test/fictional.dex test/fictional.html | diff test/fictional.json - && echo "    success."
	@echo "function-magic..."; ./dexter test/function-magic.dex test/function-magic.html | diff test/function-magic.json - && echo "    success."
	@echo "malformed-expr..."; ./dexter test/malformed-expr.dex test/malformed-expr.html | diff test/malformed-expr.json - && echo "    success."
	@echo "malformed-json..."; ./dexter test/malformed-json.dex test/malformed-json.html | diff test/malformed-json.json - && echo "    success."
	@echo "match..."; ./dexter -x test/match.dex test/match.xml | diff test/match.json - && echo "    success."
	@echo "position..."; ./dexter test/position.dex test/position.html | diff test/position.json - && echo "    success."
	@echo "remote..."; ./dexter test/remote.dex test/remote.html | diff test/remote.json - && echo "    success."
	@echo "replace..."; ./dexter -x test/replace.dex test/replace.xml | diff test/replace.json - && echo "    success."
	@echo "scope..."; ./dexter test/scope.dex test/scope.html | diff test/scope.json - && echo "    success."
	@echo "test..."; ./dexter -x test/test.dex test/test.xml | diff test/test.json - && echo "    success."
	@echo "yelp..."; ./dexter test/yelp.dex test/yelp.html | diff test/yelp.json - && echo "    success."
	