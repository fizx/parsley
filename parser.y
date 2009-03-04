%{
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "kstring.h"
#include <libxml/hash.h>

#ifndef PARSER_Y_H_INCLUDED
#define PARSER_Y_H_INCLUDED

#define YYSTYPE char *

static char* parsed_answer;

int yylex (void);
void yyerror (char const *);

void prepare_parse(char*);
void cleanup_parse(void);
void start_debugging(void);

static xmlHashTablePtr alias_hash;

char* xpath_alias(char*);
void init_xpath_alias();

int yyparse(void);
char* myparse(char*);
void answer(char*);

#endif
  
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
%token DASH
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

Root
	:	Expr OptS		{ answer($1); }
	;

LocationPath
  : RelativeLocationPath  %dprec 1
	| AbsoluteLocationPath  %dprec 1
	| selectors_group				%dprec 3
	;
	
AbsoluteLocationPath
  : SLASH RelativeLocationPath				{ $$ = astrcat($1, $2); }
	| SLASH
	| AbbreviatedAbsoluteLocationPath
	;
	
RelativeLocationPath
  : Step 
	| RelativeLocationPath SLASH Step		{ $$ = astrcat3($1, $2, $3); }
	| AbbreviatedRelativeLocationPath
	;
	
Step
  : AxisSpecifier NodeTest						{ $$ = astrcat($1, $2); }
	| AxisSpecifier NodeTest Predicate  { $$ = astrcat3($1, $2, $3); }
	| AbbreviatedStep
	;
	
AxisSpecifier
  : AxisName DBLCOLON									{ $$ = astrcat($1, $2); } 
	| AbbreviatedAxisSpecifier
	;
AxisName
	: XANCESTOR 		
	| XANCESTORSELF	
	| XATTR					
	| XCHILD				
	| XDESC					
	| XDESCSELF			
	| XFOLLOW				
	| XFOLLOWSIB		
	| XNS						
	| XPARENT				
	| XPRE					
	| XPRESIB				
	| XSELF					
	;
	
NodeTest
  : NameTest 
	| NodeType LPAREN RPAREN						 { $$ = astrcat3($1, $2, $3); }
	| XPI LPAREN Literal RPAREN					 { $$ = astrcat4($1, $2, $3, $4); }
	;
	
Predicate
  : LBRA PredicateExpr RBRA						 { $$ = astrcat3($1, $2, $3); }
	;
	
PredicateExpr
  : Expr
	;
	
AbbreviatedAbsoluteLocationPath
  : DBLSLASH RelativeLocationPath			{ $$ = astrcat($1, $2); }
	;
	
AbbreviatedRelativeLocationPath
  : RelativeLocationPath DBLSLASH Step	{ $$ = astrcat3($1, $2, $3); }
	;
	
AbbreviatedStep
  : DOT
 	| DBLDOT
	;
	
AbbreviatedAxisSpecifier
  : AT
	|				{ $$ = ""; }
	;
Expr
	: LPAREN Expr RPAREN %dprec 2  { $$ = $2; }  
  | OrExpr						 %dprec 1
	;
PrimaryExpr
  : VariableReference 
	| LPAREN Expr RPAREN 									{ $$ = astrcat3($1, $2, $3); }
	| Literal
  | Number 
	| FunctionCall
	;
	
FunctionCall
  : FunctionName LPAREN Arguments RPAREN		{ $$ = astrcat4(xpath_alias($1), $2, $3, $4); }
	;
Arguments
	: ArgumentSet
	|										{ $$ = ""; }
	;
ArgumentSet
	:	Argument COMMA ArgumentSet		%dprec 2		{ $$ = astrcat3($1, $2, $3); }
	| Argument												%dprec 1
	;
Argument
  : OptS Expr OptS		{ $$ = $2; }
	;
UnionExpr
  : PathExpr 
	| UnionExpr PIPE PathExpr							{ $$ = astrcat3($1, $2, $3); }
	;
	
PathExpr
  : LocationPath 
	| FilterExpr
	| FilterExpr SLASH RelativeLocationPath					{ $$ = astrcat3($1, $2, $3); }
	| FilterExpr DBLSLASH RelativeLocationPath				{ $$ = astrcat3($1, $2, $3); }		
	;
	
FilterExpr
  : PrimaryExpr 
	| FilterExpr Predicate				{ $$ = astrcat($1, $2); }
	;
	
OrExpr
  : AndExpr 
	| OrExpr XOR AndExpr						{ $$ = astrcat3($1, $2, $3); }
	;
	
AndExpr
  : EqualityExpr 
	| AndExpr XAND EqualityExpr			{ $$ = astrcat3($1, $2, $3); }
	;
	
EqualityExpr
  : RelationalExpr 
	| EqualityExpr EQ RelationalExpr				{ $$ = astrcat3($1, $2, $3); }
	| EqualityExpr CXOPNE RelationalExpr		{ $$ = astrcat3($1, $2, $3); }	
	;
	
RelationalExpr
  : AdditiveExpr
  | RelationalExpr LT AdditiveExpr        { $$ = astrcat3($1, $2, $3); }
  | RelationalExpr GT AdditiveExpr        { $$ = astrcat3($1, $2, $3); }
  | RelationalExpr LTE AdditiveExpr       { $$ = astrcat3($1, $2, $3); }
  | RelationalExpr GTE AdditiveExpr       { $$ = astrcat3($1, $2, $3); }
	;
	
AdditiveExpr
  : MultiplicativeExpr
	| AdditiveExpr PLUS MultiplicativeExpr		{ $$ = astrcat3($1, $2, $3); }
	| AdditiveExpr DASH MultiplicativeExpr		{ $$ = astrcat3($1, $2, $3); }
	;
	
MultiplicativeExpr
  : UnaryExpr
  | MultiplicativeExpr MultiplyOperator UnaryExpr		{ $$ = astrcat3($1, $2, $3); }
  | MultiplicativeExpr XDIV UnaryExpr              { $$ = astrcat3($1, $2, $3); }
  | MultiplicativeExpr XMOD UnaryExpr              { $$ = astrcat3($1, $2, $3); }
	;
	
UnaryExpr
  : UnionExpr 
	| DASH UnaryExpr		{ $$ = astrcat($1, $2); }
	;
	
ExprToken
  : LPAREN
	| RPAREN
	| LBRA
	| RBRA
	| DOT
	| DBLDOT
	| AT
	| COMMA
	| DBLCOLON
	| NameTest 
	| NodeType 
	| Operator 
	| FunctionName
	| AxisName 
	| Literal 
	| Number 
	| VariableReference
	;
	
Literal
  : STRING
	;
Number
  : NUMBER
	| NUMBER DOT						{ $$ = astrcat($1, $2); }
	| NUMBER DOT NUMBER			{ $$ = astrcat3($1, $2, $3); }
	| DOT NUMBER						{ $$ = astrcat($1, $2); }
	;
	
Operator
  : OperatorName 
	| MultiplyOperator 
	| SLASH
	| DBLSLASH
	| PIPE
	| PLUS
	| DASH
	| EQ
	| CXOPNE
	| LT
	| LTE
	| GT 
	| GTE
	;
	
OperatorName
  : XAND 
	| XOR 
	| XMOD 
	| XDIV
	;
	
MultiplyOperator
  : SPLAT
	;
	
VariableReference
  : DOLLAR QName				{	$$ = astrcat($1, $2); }
	;
	
NameTest
  : SPLAT
	| NCName COLON SPLAT 	{ $$ = astrcat3($1, $2, $3); }
	| QName
	;
NodeType
  : XCOMMENT 
	| XTEXT 
	| XPI 
	| XNODE
	;
	
ExprWhitespace
  : S

FunctionName
  : QName
	;

QName
	: PrefixedName
	| UnprefixedName
	;

PrefixedName
	: Prefix COLON LocalPart  { $$ = astrcat3($1, $2, $3); }
	;
	
UnprefixedName
	: LocalPart
	;

Prefix
	: NCName
	;

LocalPart
	: NCName
	;

NCName
	: NAME
	;

selectors_group
	: attribute_extended_selector COMMA OptS selectors_group		{ $$ = astrcat4(".//", $1, "|", $4); }
	| attribute_extended_selector 	{ $$ = astrcat(".//", $1); }
  ;

attribute_extended_selector
	: selector
	| selector S AT NAME	{ $$ = astrcat3($1, "/@", $4); }
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
	| possibly_empty_sequence HASH Ident		{ $$ = astrcat4($1, "[@id='", $3,"']"); }
	| possibly_empty_sequence DOT Ident		{ $$ = astrcat4($1, "[contains(concat( ' ', @class, ' ' ), concat( ' ', '", $3, "', ' ' ))]"); }
	| possibly_empty_sequence LBRA	type_selector RBRA { $$ = astrcat4($1, "[@", $3, "]"); }
	| possibly_empty_sequence LBRA	type_selector OptS EQ OptS StringLike OptS RBRA { $$ = astrcat6($1, "[@", $3, " = ", $7, "]"); }
	| possibly_empty_sequence LBRA	type_selector OptS CXOPHE OptS StringLike OptS RBRA { $$ = astrcat10($1, "[@", $3, " = ", $7, " or starts-with(@", $3, ", concat(", $7, ", '-' ))]"); }
	| possibly_empty_sequence LBRA	type_selector OptS CXOPNE OptS StringLike OptS RBRA { $$ = astrcat6($1, "[@", $3, " != ", $7, "]"); }
	| possibly_empty_sequence LBRA	type_selector OptS CXOPSTARTEQ OptS StringLike OptS RBRA { $$ = astrcat6($1, "[starts-with(@", $3, ", ", $7, ")]"); }
	| possibly_empty_sequence LBRA	type_selector OptS CXOPENDEQ OptS StringLike OptS RBRA { $$ = astrcat6($1, "[ends-with(@", $3, ", ", $7, ")]"); }
	| possibly_empty_sequence LBRA	type_selector OptS CXOPCONTAINS OptS StringLike OptS RBRA { $$ = astrcat6($1, "[contains(@", $3, ", ", $7, ")]"); }
	| possibly_empty_sequence LBRA	type_selector OptS CXOPCONTAINS2 OptS StringLike OptS RBRA { $$ = astrcat6($1, "[contains(@", $3, ", ", $7, ")]"); }
	| possibly_empty_sequence CXFIRST	{ $$ = astrcat($1, "[1]"); }
	| possibly_empty_sequence CXLAST		{ $$ = astrcat($1, "[last()]"); }
	| possibly_empty_sequence CXNOT LPAREN selectors_group RPAREN		{ $$ = astrcat5("set-difference(", $1, ", ", $4, ")"); }
	| possibly_empty_sequence CXEVEN		{ $$ = astrcat($1, "[position() % 2 = 0]"); }
	| possibly_empty_sequence CXODD		{ $$ = astrcat($1, "[position() % 2 = 1]"); }
	| possibly_empty_sequence CXEQ LPAREN NumberLike RPAREN			{ $$ = astrcat4($1, "[position() = ", $4, "]"); }
	| possibly_empty_sequence CXGT LPAREN NumberLike RPAREN			{ $$ = astrcat4($1, "[position() > ", $4, "]"); }
	| possibly_empty_sequence CXLT LPAREN NumberLike RPAREN			{ $$ = astrcat4($1, "[position() < ", $4, "]"); }
	| possibly_empty_sequence CXHEADER		{ $$ = astrcat($1, "[contains('h1 h2 h3 h4 h5 h6', lower-case(local-name()))]"); }
	| possibly_empty_sequence CXCONTAINS	LPAREN StringLike RPAREN { $$ = astrcat4($1, "[contains(., ", $4, "]"); }
	| possibly_empty_sequence CXEMPTY		{ $$ = astrcat($1, "[not(node())]"); }
	| possibly_empty_sequence CXHAS LPAREN selectors_group RPAREN		{ $$ = astrcat4($1, "[", $4, "]"); }
	| possibly_empty_sequence CXPARENT		{ $$ = astrcat($1, "[node()]"); }
	| possibly_empty_sequence CXNTHCH LPAREN NumberLike RPAREN 	{ $$ = astrcat4("*[", $4, "]/self::", $1); }
	| possibly_empty_sequence CXNTHLASTCH LPAREN NumberLike RPAREN 	{ $$ = astrcat4("*[last() - ", $4, "]/self::", $1); }
	| possibly_empty_sequence CXNTHTYPE LPAREN NumberLike RPAREN			{ $$ = astrcat4($1, "[position() = ", $4, "]"); }
	| possibly_empty_sequence CXNTHLASTTYPE LPAREN NumberLike RPAREN			{ $$ = astrcat4($1, "[position() = last() - ", $4, "]"); }
	| possibly_empty_sequence CXFIRSTCH 	{ $$ = astrcat("*[1]/self::", $1); }
	| possibly_empty_sequence CXLASTCH 	{ $$ = astrcat("*[last()]/self::", $1); }
	| possibly_empty_sequence CXFIRSTTYPE 	{ $$ = astrcat($1, "[1]"); }
	| possibly_empty_sequence CXLASTTYPE  	{ $$ = astrcat($1, "[last()]"); }
	| possibly_empty_sequence CXONLYCH 		{ $$ = astrcat("*[count()=1]/self::", $1); }
	| possibly_empty_sequence CXONLYTYPE 	{ $$ = astrcat($1, "[count()=1]"); }
	| possibly_empty_sequence CXINPUT 			{ $$ = astrcat($1, "[lower-case(name)='input']"); }
	| possibly_empty_sequence CXTEXT 			{ $$ = astrcat($1, "[lower-case(name)='input' and lower-case(@type)='text']"); }
	| possibly_empty_sequence CXPASSWORD 	{ $$ = astrcat($1, "[lower-case(name)='input' and lower-case(@type)='password']"); }
	| possibly_empty_sequence CXRADIO 			{ $$ = astrcat($1, "[lower-case(name)='input' and lower-case(@type)='radio']"); }
	| possibly_empty_sequence CXCHECKBOX 	{ $$ = astrcat($1, "[lower-case(name)='input' and lower-case(@type)='checkbox']"); }
	| possibly_empty_sequence CXSUBMIT 		{ $$ = astrcat($1, "[lower-case(name)='input' and lower-case(@type)='submit']"); }
	| possibly_empty_sequence CXIMAGE 			{ $$ = astrcat($1, "[lower-case(name)='input' and lower-case(@type)='image']"); }
	| possibly_empty_sequence CXRESET 			{ $$ = astrcat($1, "[lower-case(name)='input' and lower-case(@type)='reset']"); }
	| possibly_empty_sequence CXBUTTON 		{ $$ = astrcat($1, "[lower-case(name)='input' and lower-case(@type)='button']"); }
	| possibly_empty_sequence CXFILE 			{ $$ = astrcat($1, "[lower-case(name)='input' and lower-case(@type)='file']"); }
	| possibly_empty_sequence CXENABLED 		{ $$ = astrcat($1, "[lower-case(name)='input' and not(@disabled)]"); }
	| possibly_empty_sequence CXDISABLED 	{ $$ = astrcat($1, "[lower-case(name)='input' and @disabled]"); }
	| possibly_empty_sequence CXCHECKED 		{ $$ = astrcat($1, "[@checked]"); }
	| possibly_empty_sequence CXSELECTED 	{ $$ = astrcat($1, "[@selected]"); }
	;
	
possibly_empty_sequence
	: simple_selector_sequence
	|											{ $$ = "*"; }
	;

simple_selector_anchor
	:	type_selector
	| universal
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
	| BSLASHLIT						{ *$$ = *astrdup($1) + 1; }
	| NAME Ident					{ $$ = strcat($1, $2); }
	| BSLASHLIT Ident			{ *$$ = (*astrcat($1, $2) + 1); }
	| keyword
	;
	
keyword
	: XANCESTOR
	| XANCESTORSELF
	| XATTR
	| XCHILD
	| XDESC
	| XDESCSELF
	| XFOLLOW
	| XFOLLOWSIB
	| XNS
	| XPARENT
	| XPRE
	| XPRESIB
	| XSELF
	| XOR
	| XAND
	| XDIV
	| XMOD
	| XCOMMENT
	| XTEXT
	| XPI
	| XNODE
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

char* xpath_alias(char* key) {
	char* value = (char*) xmlHashLookup(alias_hash, key);
	return value == NULL ? key : value;
}

void init_xpath_alias() {
	alias_hash = xmlHashCreate(100);
	xmlHashAddEntry(alias_hash, "html", "lib:html-document");
	xmlHashAddEntry(alias_hash, "match", "regexp:match");
	xmlHashAddEntry(alias_hash, "replace", "regexp:replace");
	xmlHashAddEntry(alias_hash, "test", "regexp:test");
	xmlHashAddEntry(alias_hash, "with-newlines", "lib:nl");
	
}

char* myparse(char* string){
	// start_debugging();
  prepare_parse(string);
  yyparse();
  cleanup_parse();
	return parsed_answer;
}

void answer(char* a){
	parsed_answer = a;
}

void start_debugging(){
  yydebug = 1;
  return;
}