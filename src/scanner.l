%{
#include <stdio.h>
#include "kstring.h"
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
%x DSTR
%x SSTR

NUMBER      [0-9]+
S		        [\t\r \n]+
AT          "@"
LPAREN      "("
RPAREN      ")"
PIPE        "|"
LT          "<"
SLASH       "/"
DBLSLASH     "//"
BANG        "!"
COLON       ":"
DBLCOLON			"::"
QUERY       "?"
HASH        "#"
COMMA       ","
DOT         "."
DBLDOT       ".."
GT          ">"
LBRA        "["
RBRA        "]"
TILDE       "~"
SPLAT       "*"
PLUS        "+"
EQ          "="
LTE         "<="
GTE         ">="
DOLLAR      "$"
STRING			(['][^']*['])|(["][^"]*["])

BSLASHLIT   \\(.|\n)
OTHER       (.|\n)

XANCESTOR 		"ancestor"
XANCESTORSELF	"ancestor-or-self"
XATTR					"attribute"
XCHILD				"child"
XDESC					"descendant"
XDESCSELF			"descendant-or-self"
XFOLLOW				"following"
XFOLLOWSIB		"following-sibling"
XNS						"namespace"
XPARENT				"parent"
XPRE					"preceding"
XPRESIB				"preceding-sibling"
XSELF					"self"
XOR						"or"
XAND					"and"
XDIV					"div"
XMOD					"mod"

CXEQUATION    [0-9]+n
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
CXNTHLASTCH   ":nth-last-child"
CXNTHTYPE 	  ":nth-of-type"
CXNTHLASTTYPE ":nth-last-of-type"
CXFIRSTCH     ":first-child"
CXLASTCH      ":last-child"
CXFIRSTTYPE   ":first-of-type"
CXLASTTYPE    ":last-of-type"
CXONLYCH      ":only-child"
CXONLYTYPE    ":only-of-type"
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

NAME        [a-zA-Z_][a-zA-Z0-9\-_]*

%%
{NUMBER} yylval = strdup(yytext); return NUMBER;
{S} yylval = strdup(yytext); return S;
{AT} yylval = strdup(yytext); return AT;
{LPAREN} yylval = strdup(yytext); return LPAREN;
{RPAREN} yylval = strdup(yytext); return RPAREN;
{PIPE} yylval = strdup(yytext); return PIPE;
{LT} yylval = strdup(yytext); return LT;
{SLASH} yylval = strdup(yytext); return SLASH;
{DBLSLASH} yylval = strdup(yytext); return DBLSLASH;
{BANG} yylval = strdup(yytext); return BANG;
{COLON} yylval = strdup(yytext); return COLON;
{DBLCOLON} yylval = strdup(yytext); return DBLCOLON;
{QUERY} yylval = strdup(yytext); return QUERY;
{HASH} yylval = strdup(yytext); return HASH;
{COMMA} yylval = strdup(yytext); return COMMA;
{DOT} yylval = strdup(yytext); return DOT;
{DBLDOT} yylval = strdup(yytext); return DBLDOT;
{GT} yylval = strdup(yytext); return GT;
{LBRA} yylval = strdup(yytext); return LBRA;
{RBRA} yylval = strdup(yytext); return RBRA;
{TILDE} yylval = strdup(yytext); return TILDE;
{SPLAT} yylval = strdup(yytext); return SPLAT;
{PLUS} yylval = strdup(yytext); return PLUS;
{EQ} yylval = strdup(yytext); return EQ;
{LTE} yylval = strdup(yytext); return LTE;
{GTE} yylval = strdup(yytext); return GTE;
{DOLLAR} yylval = strdup(yytext); return DOLLAR;
{BSLASHLIT} yylval = strdup(yytext); return BSLASHLIT;
{OTHER} yylval = strdup(yytext); return OTHER;
{XANCESTOR} yylval = strdup(yytext); return XANCESTOR;
{XANCESTORSELF} yylval = strdup(yytext); return XANCESTORSELF;
{XATTR} yylval = strdup(yytext); return XATTR;
{XCHILD} yylval = strdup(yytext); return XCHILD;
{XDESC} yylval = strdup(yytext); return XDESC;
{XDESCSELF} yylval = strdup(yytext); return XDESCSELF;
{XFOLLOW} yylval = strdup(yytext); return XFOLLOW;
{XFOLLOWSIB} yylval = strdup(yytext); return XFOLLOWSIB;
{XNS} yylval = strdup(yytext); return XNS;
{XPARENT} yylval = strdup(yytext); return XPARENT;
{XPRE} yylval = strdup(yytext); return XPRE;
{XPRESIB} yylval = strdup(yytext); return XPRESIB;
{XSELF} yylval = strdup(yytext); return XSELF;
{XOR} yylval = strdup(yytext); return XOR;
{XAND} yylval = strdup(yytext); return XAND;
{XDIV} yylval = strdup(yytext); return XDIV;
{XMOD} yylval = strdup(yytext); return XMOD;
{CXEQUATION} yylval = strdup(yytext); return CXEQUATION;
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
{CXNTHLASTCH} yylval = strdup(yytext); return CXNTHLASTCH;
{CXNTHTYPE} yylval = strdup(yytext); return CXNTHTYPE;
{CXNTHLASTTYPE} yylval = strdup(yytext); return CXNTHLASTTYPE;
{CXFIRSTCH} yylval = strdup(yytext); return CXFIRSTCH;
{CXLASTCH} yylval = strdup(yytext); return CXLASTCH;
{CXFIRSTTYPE} yylval = strdup(yytext); return CXFIRSTTYPE;
{CXLASTTYPE} yylval = strdup(yytext); return CXLASTTYPE;
{CXONLYCH} yylval = strdup(yytext); return CXONLYCH;
{CXONLYTYPE} yylval = strdup(yytext); return CXONLYTYPE;
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
{NAME} yylval = strdup(yytext); return NAME;
{STRING} yylval = strdup(yytext); return STRING;
