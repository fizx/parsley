AC_PREREQ(2.59)

AC_INIT([parsleyc], [1.0], [kyle@kylemaxwell.com])
AM_INIT_AUTOMAKE([1.9 foreign])

LDFLAGS="$LDFLAGS -L/opt/local/lib -L/usr/local/lib"
CPPFLAGS="$CPPFLAGS -I/opt/local/include -I/usr/local/include"

AC_PROG_CC_C99
AM_PROG_LEX
AC_PROG_YACC
AC_PROG_LIBTOOL
AC_CHECK_HEADER(pcre.h, , AC_MSG_ERROR([could not find pcre.h]))
AC_CHECK_HEADER(argp.h, , AC_MSG_ERROR([could not find argp.h]))
AC_CHECK_HEADER(json/json.h, , AC_MSG_ERROR([could not find json.h]))
AC_CHECK_HEADER(json/printbuf.h, , AC_MSG_ERROR([could not find printbuf.h]))

AC_PATH_PROG(XML2_CONFIG, xml2-config, , [$PATH])
if test x$XML2_CONFIG = x ; then
  AC_MSG_ERROR([libxml2 not present or not configured])
else
  XML2_CFLAGS="`$XML2_CONFIG --cflags`"
  XML2_LIBS="`$XML2_CONFIG --libs`"
fi

AC_PATH_PROG(XSLT_CONFIG, xslt-config, , [$PATH])
if test x$XSLT_CONFIG = x ; then
  AC_MSG_ERROR([libxslt not present or not configured])
else
  XSLT_CFLAGS="`$XSLT_CONFIG --cflags`"
  XSLT_LIBS="`$XSLT_CONFIG --libs`"
fi

CPPFLAGS="$CPPFLAGS $XML2_CFLAGS $XSLT_CFLAGS"
LIBS="$LIBS $XML2_LIBS $XSLT_LIBS"

AC_CHECK_LIB(pcre, pcre_compile, , AC_MSG_ERROR([could not find pcre]))

AC_CHECK_LIB(json, json_object_new_string, , AC_MSG_ERROR([could not find the json library]))
AC_CHECK_FUNC(argp_parse, , [AC_CHECK_LIB(argp, argp_parse, , AC_MSG_ERROR([could not find argp]))])

AC_CHECK_LIB(xslt, xsltApplyStylesheet, , AC_MSG_ERROR([could not find libxslt]))
AC_CHECK_LIB(xml2, htmlParseFile, , AC_MSG_ERROR([could not find libxml2 with html parsing]))
AC_CHECK_LIB(exslt, exsltRegisterAll, , AC_MSG_ERROR([could not find libexslt]))

AC_CONFIG_FILES([Makefile])
AC_OUTPUT