AC_PREREQ(2.59)

AC_INIT([dexterc], [1.0], [kyle@kylemaxwell.com])

AM_INIT_AUTOMAKE([1.9 foreign])

LDFLAGS="$LDFLAGS -L/opt/local/lib"
CPPFLAGS="$CPPFLAGS -I/opt/local/include"

AC_PROG_CC_C99
AM_PROG_LEX
AC_PROG_YACC
AC_PROG_LIBTOOL
# AC_CHECK_HEADER(argp.h, , AC_MSG_ERROR([could not find argp.h]))
# AC_CHECK_HEADER(json/json.h, , AC_MSG_ERROR([could not find json.h]))

# AC_CHECK_HEADER(/usr/include/libxml2/libxml/HTMLtree.h, , AC_MSG_ERROR([could not find libxml2/HTMLtree.h]))
# AC_CHECK_HEADER(libexslt/exslt.h, , AC_MSG_ERROR([could not find exslt.h]))

AC_PATH_PROG(XSLT_CONFIG, xslt-config, , [$PATH])
if test x$XSLT_CONFIG = x ; then
  AC_MSG_ERROR([libxslt not present or not configured])
else
  XSLT_CFLAGS="`$XSLT_CONFIG --cflags`"
  XSLT_LIBS="`$XSLT_CONFIG --libs`"
fi

CPPFLAGS="$CPPFLAGS $XSLT_CFLAGS"
LIBS="$LIBS $XSLT_LIBS"

AC_CHECK_LIB(pcre, pcre_compile)
AC_CHECK_LIB(argp, argp_parse)
AC_CHECK_LIB(json, json_object_new_string, , AC_MSG_ERROR([could not find the json library]))

AC_CHECK_LIB(xslt, xsltApplyStylesheet)
AC_CHECK_LIB(xml, htmlParseFile)
AC_CHECK_LIB(exslt, exsltRegisterAll)

AC_CONFIG_FILES([Makefile])
AC_OUTPUT