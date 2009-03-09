%{
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "kstring.h"
#include "parsed_xpath.h"
#include <libxml/hash.h>

#ifndef PARSER_Y_H_INCLUDED
#define PARSER_Y_H_INCLUDED

static pxpathPtr parsed_answer;

int yylex (void);
void yyerror (char const *);

void prepare_parse(char*);
void cleanup_parse(void);
void start_debugging(void);

static xmlHashTablePtr alias_hash;

char* xpath_alias(char*);
void init_xpath_alias();

int yyparse(void);
pxpathPtr myparse(char*);
void answer(pxpathPtr);

#define BIN_OP     pxpath_cat_paths(3, $1, PXP($2), $3);
#define PREP_OP    pxpath_cat_paths(2, PXPIZE, $2)
#define PXPIZE     pxpath_new_path(1, $1)
#define PXP(A)     pxpath_new_path(1, A)
#define APPEND(A)  pxpath_cat_paths(2, $1, PXP(A)); 
#define PREPEND(A) pxpath_cat_paths(2, PXP(A), $1); 
#define PXPWRAP    pxpath_cat_paths(3, PXP($1), $2, PXP($3))
#define PATTERN4(A, B)      pxpath_cat_paths(4, $1, PXP(A), $3, PXP(B))
#define PATTERN4A(A, B)     pxpath_cat_paths(4, $1, PXP(A), $4, PXP(B))
#define PATTERN4B(A, B)     pxpath_cat_paths(4, PXP(A), $4, PXP(B), $1)
#define PATTERN6(A, B, C)   pxpath_cat_paths(6, $1, PXP(A), $3, PXP(B), $7, PXP(C));
#define INPUT_TYPE(A)   APPEND("[lower-case(name())='input' and lower-case(@type)='" #A "']")

#endif
  
%}

%glr-parser
%token_table
%debug

%union {
  int empty;
	char* string;
  pxpathPtr node;
}

%token <string> NUMBER
%token <string> S
%token <string> AT
%token <string> LPAREN
%token <string> RPAREN
%token <string> PIPE
%token <string> LT
%token <string> SLASH
%token <string> DBLSLASH
%token <string> BANG
%token <string> COLON
%token <string> DBLCOLON
%token <string> QUERY
%token <string> HASH
%token <string> COMMA
%token <string> DOT
%token <string> DBLDOT
%token <string> GT
%token <string> LBRA
%token <string> RBRA
%token <string> TILDE
%token <string> SPLAT
%token <string> PLUS
%token <string> DASH
%token <string> EQ
%token <string> LTE
%token <string> GTE
%token <string> DOLLAR
%token <string> BSLASHLIT
%token <string> OTHER
%token <string> XANCESTOR
%token <string> XANCESTORSELF
%token <string> XATTR
%token <string> XCHILD
%token <string> XDESC
%token <string> XDESCSELF
%token <string> XFOLLOW
%token <string> XFOLLOWSIB
%token <string> XNS
%token <string> XPARENT
%token <string> XPRE
%token <string> XPRESIB
%token <string> XSELF
%token <string> XOR
%token <string> XAND
%token <string> XDIV
%token <string> XMOD
%token <string> XCOMMENT
%token <string> XTEXT 
%token <string> XPI		
%token <string> XNODE	
%token <string> CXEQUATION
%token <string> CXOPHE
%token <string> CXOPNE
%token <string> CXOPSTARTEQ
%token <string> CXOPENDEQ
%token <string> CXOPCONTAINS
%token <string> CXOPCONTAINS2
%token <string> CXFIRST
%token <string> CXLAST
%token <string> CXNOT
%token <string> CXEVEN
%token <string> CXODD
%token <string> CXEQ
%token <string> CXGT
%token <string> CXLT
%token <string> CXHEADER
%token <string> CXCONTAINS
%token <string> CXEMPTY
%token <string> CXHAS
%token <string> CXPARENT
%token <string> CXNTHCH
%token <string> CXNTHLASTCH
%token <string> CXNTHTYPE
%token <string> CXNTHLASTTYPE
%token <string> CXFIRSTCH
%token <string> CXLASTCH
%token <string> CXFIRSTTYPE
%token <string> CXLASTTYPE
%token <string> CXONLYCH
%token <string> CXONLYTYPE
%token <string> CXINPUT
%token <string> CXTEXT
%token <string> CXPASSWORD
%token <string> CXRADIO
%token <string> CXCHECKBOX
%token <string> CXSUBMIT
%token <string> CXIMAGE
%token <string> CXRESET
%token <string> CXBUTTON
%token <string> CXFILE
%token <string> CXENABLED
%token <string> CXDISABLED
%token <string> CXCHECKED
%token <string> CXSELECTED
%token <string> NAME
%token <string> STRING
%type <node> Root
%type <empty> OptS
%type <node> LocationPath
%type <node> AbsoluteLocationPath
%type <node> RelativeLocationPath
%type <node> Step
%type <node> AxisSpecifier
%type <string> AxisName
%type <node> NodeTest
%type <node> Predicate
%type <node> PredicateExpr
%type <node> AbbreviatedAbsoluteLocationPath
%type <node> AbbreviatedRelativeLocationPath
%type <string> AbbreviatedStep
%type <string> AbbreviatedAxisSpecifier
%type <node> Expr
%type <node> NumberLike
%type <node> PrimaryExpr
%type <node> FunctionCall
%type <node> Arguments
%type <node> ArgumentSet
%type <node> PrefixedName
%type <node> Argument
%type <node> UnionExpr
%type <node> PathExpr
%type <node> FunctionName
%type <node> FilterExpr
%type <node> OrExpr
%type <node> AndExpr
%type <node> EqualityExpr
%type <node> RelationalExpr
%type <node> AdditiveExpr
%type <node> MultiplicativeExpr
%type <node> UnaryExpr
%type <string> Literal
%type <node> Number
%type <node> simple_selector_anchor
%type <string> MultiplyOperator
%type <node> VariableReference
%type <node> NameTest
%type <string> NodeType
%type <string> UnprefixedName
%type <string> combinator
%type <node> possibly_empty_sequence
%type <string> keyword
%type <node> StringLike
%type <node> selectors_group
%type <node> QName
%type <string> NCName
%type <string> Prefix
%type <string> LocalPart
%type <node> attribute_extended_selector
%type <node> Ident
%type <node> universal
%type <node> selector
%type <node> namespace_prefix
%type <node> type_selector
%type <node> element_name
%type <node> simple_selector_sequence
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
  : SLASH RelativeLocationPath				{ $$ = PREP_OP; }
	| SLASH                             { $$ = PXPIZE; }
	| AbbreviatedAbsoluteLocationPath
	;
	
RelativeLocationPath
  : Step 
  | RelativeLocationPath SLASH Step		{ $$ = BIN_OP; }
	| AbbreviatedRelativeLocationPath
	;
	
Step
  : AxisSpecifier NodeTest						{ $$ = pxpath_cat_paths(2, $1, $2); }
	| AxisSpecifier NodeTest Predicate  { $$ = pxpath_cat_paths(3, $1, $2, $3); }
	| AbbreviatedStep                   { $$ = PXPIZE; }
	;
	
AxisSpecifier
  : AxisName DBLCOLON									{ $$ = pxpath_new_path(2, $1, $2); } 
	| AbbreviatedAxisSpecifier          { $$ = PXPIZE; }
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
	| NodeType LPAREN RPAREN						 { $$ = pxpath_new_path(3, $1, $2, $3); }
	| XPI LPAREN Literal RPAREN					 { $$ = pxpath_new_path(4, $1, $2, $3, $4); }
	;
	
Predicate
  : LBRA PredicateExpr RBRA						 { $$ = PXPWRAP; }
	;
	
PredicateExpr
  : Expr
	;
	
AbbreviatedAbsoluteLocationPath
  : DBLSLASH RelativeLocationPath			{ $$ = PREP_OP; }
	;
	
AbbreviatedRelativeLocationPath
  : RelativeLocationPath DBLSLASH Step	{ $$ = BIN_OP; }
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
  : LPAREN Expr RPAREN %dprec 2  { $$ = PXPWRAP;    }  
  | OrExpr						 %dprec 1
	;
PrimaryExpr
  : VariableReference 
	| LPAREN Expr RPAREN 				 	{ $$ = PXPWRAP;     }  
	| Literal                     { $$ = PXPIZE; }
  | Number 
	| FunctionCall
	;
	
FunctionCall
  : FunctionName LPAREN Arguments RPAREN		{ $$ = pxpath_new_func(xpath_alias(pxpath_to_string($1)), $3); }
	;
Arguments
	: ArgumentSet
	|										{ $$ = NULL; }
	;
ArgumentSet
  :	Argument COMMA ArgumentSet	 	  %dprec 2		{ $$ = $1; $1->next = $2; }
	| Argument												%dprec 1
	;
Argument
  : OptS Expr OptS		{ $$ = $2; }
	;
UnionExpr
  : PathExpr 
	| UnionExpr PIPE PathExpr							{ $$ = BIN_OP; }
	;
	
PathExpr
  : LocationPath 
	| FilterExpr
	| FilterExpr SLASH RelativeLocationPath					{ $$ = BIN_OP; }
	| FilterExpr DBLSLASH RelativeLocationPath			{ $$ = BIN_OP; }
	;
	
FilterExpr
  : PrimaryExpr 
	| FilterExpr Predicate				{ $$ = pxpath_cat_paths(2, $1, $2); }
	;
	
OrExpr
  : AndExpr 
	| OrExpr XOR AndExpr						{ $$ = BIN_OP; }
	;
	
AndExpr
  : EqualityExpr 
	| AndExpr XAND EqualityExpr			{ $$ = BIN_OP; }
	;
	
EqualityExpr
  : RelationalExpr 
	| EqualityExpr EQ RelationalExpr				{ $$ = BIN_OP; }
	| EqualityExpr CXOPNE RelationalExpr		{ $$ = BIN_OP; }	
	;
	
RelationalExpr
  : AdditiveExpr
  | RelationalExpr LT AdditiveExpr        { $$ = BIN_OP; }
  | RelationalExpr GT AdditiveExpr        { $$ = BIN_OP; }
  | RelationalExpr LTE AdditiveExpr       { $$ = BIN_OP; }
  | RelationalExpr GTE AdditiveExpr       { $$ = BIN_OP; }
	;
	
AdditiveExpr
  : MultiplicativeExpr
	| AdditiveExpr PLUS MultiplicativeExpr		{ $$ = BIN_OP; }
	| AdditiveExpr DASH MultiplicativeExpr		{ $$ = BIN_OP; }
	;
	
MultiplicativeExpr
  : UnaryExpr
  | MultiplicativeExpr MultiplyOperator UnaryExpr		{ $$ = BIN_OP; }
  | MultiplicativeExpr XDIV UnaryExpr               { $$ = BIN_OP; }
  | MultiplicativeExpr XMOD UnaryExpr               { $$ = BIN_OP; }
	;
	
UnaryExpr
  : UnionExpr 
	| DASH UnaryExpr		{ $$ = PREP_OP; }
	;
	
Literal
  : STRING
	;
Number
  : NUMBER                { $$ = PXPIZE; }
	| NUMBER DOT						{ $$ = pxpath_new_path(2, $1, $2); }
	| NUMBER DOT NUMBER			{ $$ = pxpath_new_path(3, $1, $2, $3); }
	| DOT NUMBER						{ $$ = pxpath_new_path(2, $1, $2); }
	;
	
MultiplyOperator
  : SPLAT
	;
	
VariableReference
  : DOLLAR QName				{	$$ = PREP_OP; }
	;
	
NameTest
  : SPLAT               { $$ = PXPIZE; }
	| NCName COLON SPLAT 	{ $$ = pxpath_new_path(3, $1, $2, $3); }
	| QName
	;
	
NodeType
  : XCOMMENT 
	| XTEXT 
	| XPI 
	| XNODE
	;
	
FunctionName
  : QName
	;

QName
	: PrefixedName
	| UnprefixedName      { $$ = PXPIZE; }
	;

PrefixedName
	: Prefix COLON LocalPart  { $$ = pxpath_new_path(3, $1, $2, $3); }
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
	: attribute_extended_selector COMMA OptS selectors_group		{ $$ = pxpath_cat_paths(4, pxpath_new_literal(1, ".//"), $1, pxpath_new_literal(1, "|"), $4); }
	| attribute_extended_selector 	{ $$ = pxpath_cat_paths(2, pxpath_new_literal(1, ".//"), $1); }
  ;

attribute_extended_selector
	: selector
	| selector S AT NAME	{ $$ = APPEND(pxpath_new_path(2, "/@", $4)); }
	;

selector
  : simple_selector_sequence combinator selector						{ $$ = pxpath_cat_paths(3, $1, $2, $3); }
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
	| possibly_empty_sequence HASH Ident		                                                              { $$ = PATTERN4("[@id='", "']"); }
	| possibly_empty_sequence DOT Ident		                                                                { $$ = PATTERN4("[contains(concat( ' ', @class, ' ' ), concat( ' ', '",  "', ' ' ))]"); }
	| possibly_empty_sequence LBRA	type_selector RBRA                                                    { $$ = PATTERN4("[@", "]"); }
	| possibly_empty_sequence LBRA	type_selector OptS EQ OptS StringLike OptS RBRA                       { $$ = PATTERN6("[@", " = ", "]"); }
	| possibly_empty_sequence LBRA	type_selector OptS CXOPHE OptS StringLike OptS RBRA                   { $$ = pxpath_cat_paths(10, $1, PXP("[@"), $3, PXP(" = "), $7, PXP(" or starts-with(@"), $3, PXP(", concat("), $7, PXP(", '-' ))]")); }
	| possibly_empty_sequence LBRA	type_selector OptS CXOPNE OptS StringLike OptS RBRA                   { $$ = PATTERN6("[@", " != ", "]"); }
	| possibly_empty_sequence LBRA	type_selector OptS CXOPSTARTEQ OptS StringLike OptS RBRA              { $$ = PATTERN6("[starts-with(@", ", ", ")]"); }
	| possibly_empty_sequence LBRA	type_selector OptS CXOPENDEQ OptS StringLike OptS RBRA                { $$ = PATTERN6("[ends-with(@", ", ", ")]"); }
	| possibly_empty_sequence LBRA	type_selector OptS CXOPCONTAINS OptS StringLike OptS RBRA             { $$ = PATTERN6("[contains(@", ", ", ")]"); }
	| possibly_empty_sequence LBRA	type_selector OptS CXOPCONTAINS2 OptS StringLike OptS RBRA            { $$ = PATTERN6("[contains(@", ", ", ")]"); }
	| possibly_empty_sequence CXFIRST	                                                                    { $$ = APPEND("[1]"); }
	| possibly_empty_sequence CXLAST	                                                                  	{ $$ = APPEND("[last()]"); }
	| possibly_empty_sequence CXNOT LPAREN selectors_group RPAREN		                                      { $$ = pxpath_cat_paths(5, PXP("set-difference("), $1, PXP(", "), $4, PXP(")")); }
	| possibly_empty_sequence CXEVEN		                                                                  { $$ = APPEND("[position() % 2 = 0]"); }
	| possibly_empty_sequence CXODD		                                                                    { $$ = APPEND("[position() % 2 = 1]"); }
	| possibly_empty_sequence CXEQ LPAREN NumberLike RPAREN			                                          { $$ = PATTERN4A("[position() = ", "]"); }
	| possibly_empty_sequence CXGT LPAREN NumberLike RPAREN			                                          { $$ = PATTERN4A("[position() > ", "]"); }
	| possibly_empty_sequence CXLT LPAREN NumberLike RPAREN			                                          { $$ = PATTERN4A("[position() < ", "]"); }
	| possibly_empty_sequence CXHEADER		                                                                { $$ = APPEND("[contains('h1 h2 h3 h4 h5 h6', lower-case(local-name()))]"); }
	| possibly_empty_sequence CXCONTAINS	LPAREN StringLike RPAREN                                        { $$ = PATTERN4A("[contains(., ", "]"); }
	| possibly_empty_sequence CXEMPTY		                                                                  { $$ = APPEND("[not(node())]"); }
	| possibly_empty_sequence CXHAS LPAREN selectors_group RPAREN		                                      { $$ = PATTERN4A("[", "]"); }
	| possibly_empty_sequence CXPARENT		                                                                { $$ = APPEND("[node()]"); }
	| possibly_empty_sequence CXNTHCH LPAREN NumberLike RPAREN 	                                          { $$ = PATTERN4B("*[", "]/self::"); }
	| possibly_empty_sequence CXNTHLASTCH LPAREN NumberLike RPAREN 	                                      { $$ = PATTERN4B("*[last() - ", "]/self::"); }
	| possibly_empty_sequence CXNTHTYPE LPAREN NumberLike RPAREN		                                    	{ $$ = PATTERN4A("[position() = ", PXP("]"); }
	| possibly_empty_sequence CXNTHLASTTYPE LPAREN NumberLike RPAREN			                                { $$ = PATTERN4A("[position() = last() - ", "]"); }
	| possibly_empty_sequence CXFIRSTCH 	                                                                { $$ = PREPEND("*[1]/self::"); }
	| possibly_empty_sequence CXLASTCH 	                                                                  { $$ = PREPEND("*[last()]/self::"); }
	| possibly_empty_sequence CXFIRSTTYPE                                                                 { $$ = APPEND("[1]"); }
	| possibly_empty_sequence CXLASTTYPE                                                                  { $$ = APPEND("[last()]"); }
	| possibly_empty_sequence CXONLYCH 		                                                                { $$ = PREPEND("*[count()=1]/self::"); }
	| possibly_empty_sequence CXONLYTYPE 	                                                                { $$ = APPEND("[count()=1]"); }
	| possibly_empty_sequence CXINPUT 		                                                                { $$ = APPEND("[lower-case(name())='input']"); }
  | possibly_empty_sequence CXTEXT 			                                                                { $$ = INPUT_TYPE(text); }
	| possibly_empty_sequence CXPASSWORD 	                                                                { $$ = INPUT_TYPE(password); }
	| possibly_empty_sequence CXRADIO 		                                                                { $$ = INPUT_TYPE(radio); }
	| possibly_empty_sequence CXCHECKBOX 	                                                                { $$ = INPUT_TYPE(checkbox); }
	| possibly_empty_sequence CXSUBMIT 		                                                                { $$ = INPUT_TYPE(submit); }
	| possibly_empty_sequence CXIMAGE 		                                                                { $$ = INPUT_TYPE(image); }
	| possibly_empty_sequence CXRESET 		                                                                { $$ = INPUT_TYPE(reset); }
	| possibly_empty_sequence CXBUTTON 		                                                                { $$ = INPUT_TYPE(button); }
	| possibly_empty_sequence CXFILE 			                                                                { $$ = INPUT_TYPE(file); }
	| possibly_empty_sequence CXENABLED 	                                                                { $$ = APPEND("[lower-case(name())='input' and not(@disabled)]"); }
	| possibly_empty_sequence CXDISABLED 	                                                                { $$ = APPEND("[lower-case(name())='input' and @disabled]"); }
	| possibly_empty_sequence CXCHECKED 	                                                                { $$ = APPEND("[@checked]"); }
	| possibly_empty_sequence CXSELECTED 	                                                                { $$ = APPEND("[@selected]"); }
	;
	
possibly_empty_sequence
	: simple_selector_sequence
	|											{ $$ = PXP("*"); }
	;

simple_selector_anchor
	:	type_selector
	| universal
	;

type_selector
  : namespace_prefix element_name	{	$$ = pxpath_cat_paths(3, $1, PXP(":"), $2); }
  | element_name				
  ;

namespace_prefix
  : SPLAT PIPE			{ $$ = PXP("*"); }
  | Ident PIPE			{	$$ = $1;  }
  | PIPE						{ $$ = PXP("*"); }
  ;

element_name
  : Ident
  ;

universal
  : namespace_prefix SPLAT { $$ = astrcat3($1, ":", $2); }
  | SPLAT                   { $$ = PXPIZE; }
  ;

NumberLike
	: NUMBER                  { $$ = PXPIZE; }
  ;

Ident
	: NAME                { $$ = PXPIZE; }
	| BSLASHLIT						{ $$ = pxpath_new_literal(1, &(*$1 + 1)); }
	| NAME Ident					{ $$ = pxpath_new_literal(2, $1, $2); }
	| BSLASHLIT Ident			{ $$ = pxpath_new_literal(2, "\\", $2); }
	| keyword             { $$ = PXPIZE; }
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
	: Ident 			
	| STRING      { $$ = pxpath_new_literal(1, $1); }
	;

OptS
  : S			      { $$ = 0; }					
	|							{ $$ = 0; }
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
	return pxpath_to_string(parsed_answer);
}

void answer(char* a){
	parsed_answer = a;
}

void start_debugging(){
  yydebug = 1;
  return;
}   