%{
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "kstring.h"

#define YYSTYPE char *

int yylex (void);
void yyerror (char const *);

void prepare_parse(char*);
void cleanup_parse(void);
void start_debugging(void);

int yyparse(void);
void myparse(char*);
  
%}

%glr-parser
%token_table
%debug


%token NUMBER
%token S
%token AT
%token LPAREN
%token RPAREN
%token PIPE
%token LT
%token SLASH
%token DBLSLASH
%token BANG
%token COLON
%token DBLCOLON
%token QUERY
%token HASH
%token COMMA
%token DOT
%token DBLDOT
%token GT
%token LBRA
%token RBRA
%token TILDE
%token SPLAT
%token PLUS
%token EQ
%token LTE
%token GTE
%token DOLLAR
%token BSLASHLIT
%token OTHER
%token XANCESTOR
%token XANCESTORSELF
%token XATTR
%token XCHILD
%token XDESC
%token XDESCSELF
%token XFOLLOW
%token XFOLLOWSIB
%token XNS
%token XPARENT
%token XPRE
%token XPRESIB
%token XSELF
%token XOR
%token XAND
%token XDIV
%token XMOD
%token CXEQUATION
%token CXOPHE
%token CXOPNE
%token CXOPSTARTEQ
%token CXOPENDEQ
%token CXOPCONTAINS
%token CXOPCONTAINS2
%token CXFIRST
%token CXLAST
%token CXNOT
%token CXEVEN
%token CXODD
%token CXEQ
%token CXGT
%token CXLT
%token CXHEADER
%token CXCONTAINS
%token CXEMPTY
%token CXHAS
%token CXPARENT
%token CXHIDDEN
%token CXVISIBLE
%token CXNTHCH
%token CXNTHLASTCH
%token CXNTHTYPE
%token CXNTHLASTTYPE
%token CXFIRSTCH
%token CXLASTCH
%token CXFIRSTTYPE
%token CXLASTTYPE
%token CXONLYCH
%token CXONLYTYPE
%token CXINPUT
%token CXTEXT
%token CXPASSWORD
%token CXRADIO
%token CXCHECKBOX
%token CXSUBMIT
%token CXIMAGE
%token CXRESET
%token CXBUTTON
%token CXFILE
%token CXENABLED
%token CXDISABLED
%token CXCHECKED
%token CXSELECTED
%token NAME
%token STRING

%%

main
	: NAME
	;
	
%%

void yyerror (const char * s) {
  printf("%s\n", s);
  exit(1);
}

void myparse(char* string){
  prepare_parse(string);
  yyparse();
  cleanup_parse();
}


void start_debugging(){
  yydebug = 1;
  return;
}