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
%token XCOMMENT
%token XTEXT 
%token XPI		
%token XNODE	
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
final
	: selectors_full_group						{ printf("-----\nXPath: %s\n-----\n", $1); }
	;
	
selectors_full_group
	: OptS selectors_group OptS				{ $$ = astrcat(".//", $2); }
	;
	
selectors_group
	: selector COMMA OptS selectors_group		{ $$ = astrcat3($1, "|", $4); }
	| selector 
  ;

selector
  : simple_selector_sequence combinator selector						{ $$ = astrcat3($1, $2, $3); }
	| simple_selector_sequence
  ;
	
combinator
  : PLUS OptS			{ $$ = "/following-sibling::*[1]/self::"; } 
	| GT OptS				{ $$ = "/"; } 
	| TILDE OptS		{ $$ = "/following-sibling::*/self::"; } 
	| S							{ $$ = "//"; }
  ;

simple_selector_sequence
	: simple_selector_anchor
	| simple_selector_sequence HASH Ident		{ $$ = astrcat4($1, "[@id=\"", $3,"\"]"); }
	| simple_selector_sequence DOT Ident		{ $$ = astrcat4($1, "DIV[ contains(concat( ' ', @class, ' ' ), concat( ' ', '", $3, "', ' ' ))]"); }
	| simple_selector_sequence LBRA	type_selector RBRA { $$ = astrcat4($1, "[@", $3, "]"); }
	| simple_selector_sequence LBRA	type_selector OptS EQ OptS StringLike OptS RBRA { $$ = astrcat6($1, "[@", $3, " = ", $7, "]"); }
	| simple_selector_sequence LBRA	type_selector OptS CXOPHE OptS StringLike OptS RBRA { $$ = astrcat10($1, "[@", $3, " = '", $7, "' or starts-with(@", $3, ", concat('", $7, "', '-' ))]"); }
	| simple_selector_sequence LBRA	type_selector OptS CXOPNE OptS StringLike OptS RBRA { $$ = astrcat6($1, "[@", $3, " != ", $7, "]"); }
	| simple_selector_sequence LBRA	type_selector OptS CXOPSTARTEQ OptS StringLike OptS RBRA { $$ = astrcat6($1, "[starts-with(@", $3, ", ", $7, ")]"); }
	| simple_selector_sequence LBRA	type_selector OptS CXOPENDEQ OptS StringLike OptS RBRA { $$ = astrcat6($1, "[ends-with(@", $3, ", ", $7, ")]"); }
	| simple_selector_sequence LBRA	type_selector OptS CXOPCONTAINS OptS StringLike OptS RBRA { $$ = astrcat6($1, "[contains(@", $3, ", ", $7, ")]"); }
	| simple_selector_sequence LBRA	type_selector OptS CXOPCONTAINS2 OptS StringLike OptS RBRA { $$ = astrcat6($1, "[contains(@", $3, ", ", $7, ")]"); }
	| simple_selector_sequence CXFIRST	{ $$ = astrcat($1, "[1]"); }
	| simple_selector_sequence CXLAST		{ $$ = astrcat($1, "[last()]"); }
	| simple_selector_sequence CXNOT LPAREN selectors_full_group RPAREN		{ $$ = astrcat5("set-difference(", $1, ", ", $4, ")"); }
	| simple_selector_sequence CXEVEN		{ $$ = astrcat($1, "[position() % 2 = 0]"); }
	| simple_selector_sequence CXODD		{ $$ = astrcat($1, "[position() % 2 = 1]"); }
	| simple_selector_sequence CXEQ LPAREN NumberLike RPAREN			{ $$ = astrcat4($1, "[position() = ", $4, "]"); }
	| simple_selector_sequence CXGT LPAREN NumberLike RPAREN			{ $$ = astrcat4($1, "[position() > ", $4, "]"); }
	| simple_selector_sequence CXLT LPAREN NumberLike RPAREN			{ $$ = astrcat4($1, "[position() < ", $4, "]"); }
	| simple_selector_sequence CXHEADER		{ $$ = astrcat($1, "[contains(\"h1 h2 h3 h4 h5 h6\", lower-case(local-name()))]"); }
	| simple_selector_sequence CXCONTAINS	LPAREN StringLike RPAREN { $$ = astrcat4($1, "[contains(., ", $4, "]"); }
	| simple_selector_sequence CXEMPTY		{ $$ = astrcat($1, "[not(node())]"); }
	| simple_selector_sequence CXHAS LPAREN selectors_full_group RPAREN		{ $$ = astrcat4($1, "[", $4, "]"); }
	| simple_selector_sequence CXPARENT		{ $$ = astrcat($1, "[node()]"); }
	| simple_selector_sequence CXNTHCH LPAREN NumberLike RPAREN 	{ $$ = astrcat4("*[", $4, "]/self::", $1); }
	| simple_selector_sequence CXNTHLASTCH LPAREN NumberLike RPAREN 	{ $$ = astrcat4("*[last() - ", $4, "]/self::", $1); }
	| simple_selector_sequence CXNTHTYPE LPAREN NumberLike RPAREN			{ $$ = astrcat4($1, "[position() = ", $4, "]"); }
	| simple_selector_sequence CXNTHLASTTYPE LPAREN NumberLike RPAREN			{ $$ = astrcat4($1, "[position() = last() - ", $4, "]"); }
	| simple_selector_sequence CXFIRSTCH 	{ $$ = astrcat("*[1]/self::", $1); }
	| simple_selector_sequence CXLASTCH 	{ $$ = astrcat("*[last()]/self::", $1); }
	| simple_selector_sequence CXFIRSTTYPE 	{ $$ = astrcat($1, "[1]"); }
	| simple_selector_sequence CXLASTTYPE  	{ $$ = astrcat($1, "[last()]"); }
	| simple_selector_sequence CXONLYCH 		{ $$ = astrcat("*[count()=1]/self::", $1); }
	| simple_selector_sequence CXONLYTYPE 	{ $$ = astrcat($1, "[count()=1]"); }
	| simple_selector_sequence CXINPUT 			{ $$ = astrcat($1, "[lower-case(name)='input']"); }
	| simple_selector_sequence CXTEXT 			{ $$ = astrcat($1, "[lower-case(name)='input' and lower-case(@type)='text']"); }
	| simple_selector_sequence CXPASSWORD 	{ $$ = astrcat($1, "[lower-case(name)='input' and lower-case(@type)='password']"); }
	| simple_selector_sequence CXRADIO 			{ $$ = astrcat($1, "[lower-case(name)='input' and lower-case(@type)='radio']"); }
	| simple_selector_sequence CXCHECKBOX 	{ $$ = astrcat($1, "[lower-case(name)='input' and lower-case(@type)='checkbox']"); }
	| simple_selector_sequence CXSUBMIT 		{ $$ = astrcat($1, "[lower-case(name)='input' and lower-case(@type)='submit']"); }
	| simple_selector_sequence CXIMAGE 			{ $$ = astrcat($1, "[lower-case(name)='input' and lower-case(@type)='image']"); }
	| simple_selector_sequence CXRESET 			{ $$ = astrcat($1, "[lower-case(name)='input' and lower-case(@type)='reset']"); }
	| simple_selector_sequence CXBUTTON 		{ $$ = astrcat($1, "[lower-case(name)='input' and lower-case(@type)='button']"); }
	| simple_selector_sequence CXFILE 			{ $$ = astrcat($1, "[lower-case(name)='input' and lower-case(@type)='file']"); }
	| simple_selector_sequence CXENABLED 		{ $$ = astrcat($1, "[lower-case(name)='input' and not(@disabled)]"); }
	| simple_selector_sequence CXDISABLED 	{ $$ = astrcat($1, "[lower-case(name)='input' and @disabled]"); }
	| simple_selector_sequence CXCHECKED 		{ $$ = astrcat($1, "[@checked]"); }
	| simple_selector_sequence CXSELECTED 	{ $$ = astrcat($1, "[@selected]"); }
	;

simple_selector_anchor
	:	type_selector
	| universal
	|										{ $$ = "*"; }
	;

type_selector
  : namespace_prefix element_name	{	$$ = astrcat3($1, ":", $2); }
  | element_name
  ;

namespace_prefix
  : SPLAT PIPE			{ $$ = "*"; }
  | Ident PIPE			{	$$ = $1;  }
  | PIPE						{ $$ = "*"; }
  ;

element_name
  : Ident
  ;

universal
  : namespace_prefix SPLAT { $$ = astrcat3($1, ":", $2); }
  | SPLAT
  ;

NumberLike
	: NUMBER
  ;

Ident
	: NAME
	| BSLASHLIT						{ *$$ = *strdup($1) + 1; }
	| NAME Ident					{ $$ = strcat($1, $2); }
	| BSLASHLIT Ident			{ *$$ = (*astrcat($1, $2) + 1); }
	;
	
StringLike
	: Ident 			{ $$ = astrcat3("'", $1, "'"); }
	| STRING
	;

OptS
	: S								{ $$ = " "; }
	|									{ $$ = "";  }
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