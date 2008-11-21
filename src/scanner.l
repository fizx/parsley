%{
#include <stdio.h>
#include "y.tab.h"

YY_BUFFER_STATE mybuffer;

void prepare_parse(char* msg) {
  mybuffer = yy_scan_string(msg);  
}

void cleanup_parse() {
  yy_delete_buffer(mybuffer);  
}
 
%}

%option stack
%option case-insensitive

ident     [-]?{nmstart}{nmchar}*
name      {nmchar}+
nmstart   [_a-z]|{nonascii}|{escape}
nonascii  [^\0-\177]
unicode   \\[0-9a-f]{1,6}(\r\n|[ \n\r\t\f])?
escape    {unicode}|\\[^\n\r\f0-9a-f]
nmchar    [_a-z0-9-]|{nonascii}|{escape}
num       [0-9]+|[0-9]*\.[0-9]+
string    {string1}|{string2}
string1   \"([^\n\r\f\\"]|\\{nl}|{nonascii}|{escape})*\"
string2   \'([^\n\r\f\\']|\\{nl}|{nonascii}|{escape})*\'
invalid   {invalid1}|{invalid2}
invalid1  \"([^\n\r\f\\"]|\\{nl}|{nonascii}|{escape})*
invalid2  \'([^\n\r\f\\']|\\{nl}|{nonascii}|{escape})*
nl        \n|\r\n|\r|\f
w         [ \t\r\n\f]*


%%

[ \t\r\n\f]+     return S;

"~="             return INCLUDES;
"|="             return DASHMATCH;
"^="             return PREFIXMATCH;
"$="             return SUFFIXMATCH;
"*="             return SUBSTRINGMATCH;
{ident}          return IDENT;
{string}         return STRING;
{ident}"("       return FUNCTION;
{num}            return NUMBER;
"#"{name}        return HASH;
{w}"+"           return PLUS;
{w}">"           return GREATER;
{w}","           return COMMA;
{w}"~"           return TILDE;
":not("          return NOT;
@{ident}         return ATKEYWORD;
{invalid}        return INVALID;
{num}%           return PERCENTAGE;
{num}{ident}     return DIMENSION;
"<!--"           return CDO;
"-->"            return CDC;

"url("{w}{string}{w}")"                           return URI;
"url("{w}([!#$%&*-~]|{nonascii}|{escape})*{w}")"  return URI;
U\+[0-9a-f?]{1,6}(-[0-9a-f]{1,6})?                return UNICODE_RANGE;

\/\*[^*]*\*+([^/*][^*]*\*+)*\/                    /* ignore comments */

.                return *yytext;
