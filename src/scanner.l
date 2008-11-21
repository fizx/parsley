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

NUMBER      [0-9]+
NAME        [a-zA-Z][a-zA-Z0-9_]*
WHITE       [\t \n]+
AT          "@"
LPAREN      "("
RPAREN      ")"
PIPE        "|"
LT          "<"
SLASH       "/"
BANG        "!"
COLON       ":"
QUERY       "?"
HASH        "#"
COMMA       ","
DOT         "."
GT          ">"
LBRA        "["
RBRA        "]"
TILDE       "~"
SPLAT       "*"
PLUS        "+"
EQ          "="

BSLASHLIT   \\(.|\n)
OTHER       (.|\n)

CXEQUATION      [0-9]+n
CXOPHE        "|="
CXOPNE        "!="
CXOPSTARTEQ   "^="
CXOPENDEQ     "$="
CXOPCONTAINS  "*="
CXOPCONTAINS2 "~="
CXFIRST       ":first"
CXLAST        ":last"
CXNOT         ":not"
CXEVEN        ":even"
CXODD         ":odd"
CXEQ          ":eq"
CXGT          ":gt"
CXLT          ":lt"
CXHEADER      ":header"
CXCONTAINS    ":contains"
CXEMPTY       ":empty"
CXHAS         ":has"
CXPARENT      ":parent"
CXHIDDEN      ":hidden"
CXVISIBLE     ":visible"
CXNTHCH       ":nth-child"
CXFIRSTCH     ":first-child"
CXLASTCH      ":last-child"
CXONLYCH      ":only-child"
CXINPUT       ":input"
CXTEXT        ":text"
CXPASSWORD    ":password"
CXRADIO       ":radio"
CXCHECKBOX    ":checkbox"
CXSUBMIT      ":submit"
CXIMAGE       ":image"
CXRESET       ":reset"
CXBUTTON      ":button"
CXFILE        ":file"
CXENABLED     ":enabled"
CXDISABLED    ":disabled"
CXCHECKED     ":checked"
CXSELECTED    ":selected"

%%

{HASH} yylval = strdup(yytext); return HASH;
{DOT} yylval = strdup(yytext); return DOT;
{NAME} yylval = strdup(yytext); return NAME;
{NUMBER} yylval = strdup(yytext); return NUMBER;
{CXEQUATION} yylval = strdup(yytext); return CXEQUATION;
{WHITE} yylval = strdup(yytext); return WHITE;
{BSLASHLIT} yylval = strdup(yytext); return BSLASHLIT;
{GT} yylval = strdup(yytext); return GT;
{LT} yylval = strdup(yytext); return LT;
{LBRA} yylval = strdup(yytext); return LBRA;
{RBRA} yylval = strdup(yytext); return RBRA;
{TILDE} yylval = strdup(yytext); return TILDE;
{COMMA} yylval = strdup(yytext); return COMMA;
{SPLAT} yylval = strdup(yytext); return SPLAT;
{PLUS} yylval = strdup(yytext); return PLUS;
{LPAREN} yylval = strdup(yytext); return LPAREN;
{RPAREN} yylval = strdup(yytext); return RPAREN;
{EQ} yylval = strdup(yytext); return EQ;
{CXOPHE} yylval = strdup(yytext); return CXOPHE;
{CXOPNE} yylval = strdup(yytext); return CXOPNE;
{CXOPSTARTEQ} yylval = strdup(yytext); return CXOPSTARTEQ;
{CXOPENDEQ} yylval = strdup(yytext); return CXOPENDEQ;
{CXOPCONTAINS} yylval = strdup(yytext); return CXOPCONTAINS;
{CXOPCONTAINS2} yylval = strdup(yytext); return CXOPCONTAINS2;
{CXFIRST} yylval = strdup(yytext); return CXFIRST;
{CXLAST} yylval = strdup(yytext); return CXLAST;
{CXNOT} yylval = strdup(yytext); return CXNOT;
{CXEVEN} yylval = strdup(yytext); return CXEVEN;
{CXODD} yylval = strdup(yytext); return CXODD;
{CXEQ} yylval = strdup(yytext); return CXEQ;
{CXGT} yylval = strdup(yytext); return CXGT;
{CXLT} yylval = strdup(yytext); return CXLT;
{CXHEADER} yylval = strdup(yytext); return CXHEADER;
{CXCONTAINS} yylval = strdup(yytext); return CXCONTAINS;
{CXEMPTY} yylval = strdup(yytext); return CXEMPTY;
{CXHAS} yylval = strdup(yytext); return CXHAS;
{CXPARENT} yylval = strdup(yytext); return CXPARENT;
{CXHIDDEN} yylval = strdup(yytext); return CXHIDDEN;
{CXVISIBLE} yylval = strdup(yytext); return CXVISIBLE;
{CXNTHCH} yylval = strdup(yytext); return CXNTHCH;
{CXFIRSTCH} yylval = strdup(yytext); return CXFIRSTCH;
{CXLASTCH} yylval = strdup(yytext); return CXLASTCH;
{CXONLYCH} yylval = strdup(yytext); return CXONLYCH;
{CXINPUT} yylval = strdup(yytext); return CXINPUT;
{CXTEXT} yylval = strdup(yytext); return CXTEXT;
{CXPASSWORD} yylval = strdup(yytext); return CXPASSWORD;
{CXRADIO} yylval = strdup(yytext); return CXRADIO;
{CXCHECKBOX} yylval = strdup(yytext); return CXCHECKBOX;
{CXSUBMIT} yylval = strdup(yytext); return CXSUBMIT;
{CXIMAGE} yylval = strdup(yytext); return CXIMAGE;
{CXRESET} yylval = strdup(yytext); return CXRESET;
{CXBUTTON} yylval = strdup(yytext); return CXBUTTON;
{CXFILE} yylval = strdup(yytext); return CXFILE;
{CXENABLED} yylval = strdup(yytext); return CXENABLED;
{CXDISABLED} yylval = strdup(yytext); return CXDISABLED;
{CXCHECKED} yylval = strdup(yytext); return CXCHECKED;
{CXSELECTED} yylval = strdup(yytext); return CXSELECTED;
{OTHER} yylval = strdup(yytext); return OTHER;
