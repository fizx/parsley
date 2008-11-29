AC_PREREQ(2.59)

AC_INIT([dexterc], [1.0], [kyle@kylemaxwell.com])

AM_INIT_AUTOMAKE([1.9 foreign])

AC_PROG_CC_C99
AM_PROG_LEX
AC_PROG_YACC
AC_PROG_LIBTOOL
AC_CHECK_HEADER(argp.h, , AC_MSG_ERROR([could not find argp.h]))
AC_CHECK_HEADER(json/json.h, , AC_MSG_ERROR([could not find json.h]))
AC_CHECK_LIB(argp, argp_parse)
AC_CHECK_LIB(json, json_object_new_string, , AC_MSG_ERROR([could not find the json library]))

AC_CONFIG_FILES([Makefile])
AC_OUTPUT