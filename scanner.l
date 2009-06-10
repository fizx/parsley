%{
#include <stdio.h>
#include "parser.h"

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
DBLCOLON		"::"
QUERY       "?"
HASH        "#"
COMMA       {S}?","
DOT         "."
DBLDOT       ".."
GT          {S}?">"
LBRA        "["
RBRA        "]"
TILDE       {S}?"~"
SPLAT       "*"
PLUS        {S}?"+"
DASH				"-"
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
XCOMMENT			"comment"
XTEXT 				"text" 
XPI					 	"processing-instruction"
XNODE					"node"
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
CXCONTENT     ":content"
CXHEADER      ":header"
CXCONTAINS    ":contains"
CXEMPTY       ":empty"
CXHAS         ":has"
CXPARENT      ":parent"
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
{NUMBER} yylval.string = strdup(yytext); return NUMBER;
{S} yylval.string = strdup(yytext); return S;
{AT} yylval.string = strdup(yytext); return AT;
{LPAREN} yylval.string = strdup(yytext); return LPAREN;
{RPAREN} yylval.string = strdup(yytext); return RPAREN;
{PIPE} yylval.string = strdup(yytext); return PIPE;
{LT} yylval.string = strdup(yytext); return LT;
{SLASH} yylval.string = strdup(yytext); return SLASH;
{DBLSLASH} yylval.string = strdup(yytext); return DBLSLASH;
{BANG} yylval.string = strdup(yytext); return BANG;
{COLON} yylval.string = strdup(yytext); return COLON;
{DBLCOLON} yylval.string = strdup(yytext); return DBLCOLON;
{QUERY} yylval.string = strdup(yytext); return QUERY;
{HASH} yylval.string = strdup(yytext); return HASH;
{COMMA} yylval.string = strdup(yytext); return COMMA;
{DOT} yylval.string = strdup(yytext); return DOT;
{DBLDOT} yylval.string = strdup(yytext); return DBLDOT;
{GT} yylval.string = strdup(yytext); return GT;
{LBRA} yylval.string = strdup(yytext); return LBRA;
{RBRA} yylval.string = strdup(yytext); return RBRA;
{TILDE} yylval.string = strdup(yytext); return TILDE;
{SPLAT} yylval.string = strdup(yytext); return SPLAT;
{PLUS} yylval.string = strdup(yytext); return PLUS;
{DASH} yylval.string = strdup(yytext); return DASH;
{EQ} yylval.string = strdup(yytext); return EQ;
{LTE} yylval.string = strdup(yytext); return LTE;
{GTE} yylval.string = strdup(yytext); return GTE;
{DOLLAR} yylval.string = strdup(yytext); return DOLLAR;
{BSLASHLIT} yylval.string = strdup(yytext); return BSLASHLIT;
{XANCESTOR} yylval.string = strdup(yytext); return XANCESTOR;
{XANCESTORSELF} yylval.string = strdup(yytext); return XANCESTORSELF;
{XATTR} yylval.string = strdup(yytext); return XATTR;
{XCHILD} yylval.string = strdup(yytext); return XCHILD;
{XDESC} yylval.string = strdup(yytext); return XDESC;
{XDESCSELF} yylval.string = strdup(yytext); return XDESCSELF;
{XFOLLOW} yylval.string = strdup(yytext); return XFOLLOW;
{XFOLLOWSIB} yylval.string = strdup(yytext); return XFOLLOWSIB;
{XNS} yylval.string = strdup(yytext); return XNS;
{XPARENT} yylval.string = strdup(yytext); return XPARENT;
{XPRE} yylval.string = strdup(yytext); return XPRE;
{XPRESIB} yylval.string = strdup(yytext); return XPRESIB;
{XSELF} yylval.string = strdup(yytext); return XSELF;
{XOR} yylval.string = strdup(yytext); return XOR;
{XAND} yylval.string = strdup(yytext); return XAND;
{XDIV} yylval.string = strdup(yytext); return XDIV;
{XMOD} yylval.string = strdup(yytext); return XMOD;
{XCOMMENT} yylval.string = strdup(yytext); return XCOMMENT;
{XTEXT} yylval.string = strdup(yytext); return XTEXT;
{XPI} yylval.string = strdup(yytext); return XPI;
{XNODE} yylval.string = strdup(yytext); return XNODE;
{CXEQUATION} yylval.string = strdup(yytext); return CXEQUATION;
{CXOPHE} yylval.string = strdup(yytext); return CXOPHE;
{CXOPNE} yylval.string = strdup(yytext); return CXOPNE;
{CXOPSTARTEQ} yylval.string = strdup(yytext); return CXOPSTARTEQ;
{CXOPENDEQ} yylval.string = strdup(yytext); return CXOPENDEQ;
{CXOPCONTAINS} yylval.string = strdup(yytext); return CXOPCONTAINS;
{CXOPCONTAINS2} yylval.string = strdup(yytext); return CXOPCONTAINS2;
{CXFIRST} yylval.string = strdup(yytext); return CXFIRST;
{CXLAST} yylval.string = strdup(yytext); return CXLAST;
{CXNOT} yylval.string = strdup(yytext); return CXNOT;
{CXEVEN} yylval.string = strdup(yytext); return CXEVEN;
{CXODD} yylval.string = strdup(yytext); return CXODD;
{CXEQ} yylval.string = strdup(yytext); return CXEQ;
{CXGT} yylval.string = strdup(yytext); return CXGT;
{CXLT} yylval.string = strdup(yytext); return CXLT;
{CXCONTENT} yylval.string = strdup(yytext); return CXCONTENT;
{CXHEADER} yylval.string = strdup(yytext); return CXHEADER;
{CXCONTAINS} yylval.string = strdup(yytext); return CXCONTAINS;
{CXEMPTY} yylval.string = strdup(yytext); return CXEMPTY;
{CXHAS} yylval.string = strdup(yytext); return CXHAS;
{CXPARENT} yylval.string = strdup(yytext); return CXPARENT;
{CXNTHCH} yylval.string = strdup(yytext); return CXNTHCH;
{CXNTHLASTCH} yylval.string = strdup(yytext); return CXNTHLASTCH;
{CXNTHTYPE} yylval.string = strdup(yytext); return CXNTHTYPE;
{CXNTHLASTTYPE} yylval.string = strdup(yytext); return CXNTHLASTTYPE;
{CXFIRSTCH} yylval.string = strdup(yytext); return CXFIRSTCH;
{CXLASTCH} yylval.string = strdup(yytext); return CXLASTCH;
{CXFIRSTTYPE} yylval.string = strdup(yytext); return CXFIRSTTYPE;
{CXLASTTYPE} yylval.string = strdup(yytext); return CXLASTTYPE;
{CXONLYCH} yylval.string = strdup(yytext); return CXONLYCH;
{CXONLYTYPE} yylval.string = strdup(yytext); return CXONLYTYPE;
{CXINPUT} yylval.string = strdup(yytext); return CXINPUT;
{CXTEXT} yylval.string = strdup(yytext); return CXTEXT;
{CXPASSWORD} yylval.string = strdup(yytext); return CXPASSWORD;
{CXRADIO} yylval.string = strdup(yytext); return CXRADIO;
{CXCHECKBOX} yylval.string = strdup(yytext); return CXCHECKBOX;
{CXSUBMIT} yylval.string = strdup(yytext); return CXSUBMIT;
{CXIMAGE} yylval.string = strdup(yytext); return CXIMAGE;
{CXRESET} yylval.string = strdup(yytext); return CXRESET;
{CXBUTTON} yylval.string = strdup(yytext); return CXBUTTON;
{CXFILE} yylval.string = strdup(yytext); return CXFILE;
{CXENABLED} yylval.string = strdup(yytext); return CXENABLED;
{CXDISABLED} yylval.string = strdup(yytext); return CXDISABLED;
{CXCHECKED} yylval.string = strdup(yytext); return CXCHECKED;
{CXSELECTED} yylval.string = strdup(yytext); return CXSELECTED;
{NAME} yylval.string = strdup(yytext); return NAME;
{STRING} yylval.string = strdup(yytext); return STRING;
{OTHER} yylval.string = strdup(yytext); return OTHER;
