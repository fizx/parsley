%{
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

#define LIT_BIN_OP(A, B, C)           pxpath_cat_literals(3, A, LIT(B), C)
#define BIN_OP(A, B, C)               pxpath_cat_paths(3, A, OP(B), C)
#define PREP_OP(A, B)                 pxpath_cat_paths(2, OP(A), B)
#define PXP(A)                        pxpath_new_path(1, A)
#define LIT(A)                        pxpath_new_literal(1, A)
#define OP(A)   	                    pxpath_new_operator(1, A)
#define APPEND(A, S)                  pxpath_cat_paths(2, A, PXP(S)); 
#define PREPEND(A, S)                 pxpath_cat_paths(2, PXP(S), A); 
#define PXPWRAP(A, B, C)              pxpath_cat_paths(3, PXP(A), B, PXP(C))
#define P4E(A, B, C, D)               pxpath_cat_paths(4, A, PXP(B), C, PXP(D))
#define P4O(A, B, C, D)               pxpath_cat_paths(4, PXP(A), B, PXP(C), D)
#define P6E(A, B, C, D, E, F)         pxpath_cat_paths(6, A, PXP(B), C, PXP(D), E, PXP(F));
#define INPUT_TYPE(A, S)              APPEND(A, "[lower-case(name())='input' and lower-case(@type)='" #S "']")
#define TRACE(A, B)                   fprintf(stderr, "trace(%s): ", A); fprintf(stderr, "%s\n", pxpath_to_string(B));

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
%token <string> CXCONTENT
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
%type <node> Predicates
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
	| AbsoluteLocationPath  %dprec 2
	| selectors_group				%dprec 3
	;
	
AbsoluteLocationPath
  : SLASH RelativeLocationPath				{ $$ = PREP_OP($1, $2); }
	| SLASH                             { $$ = PXP($1); }
	| AbbreviatedAbsoluteLocationPath
	;
	
RelativeLocationPath
  : Step 
  | RelativeLocationPath SLASH Step		{ $$ = BIN_OP($1, $2, $3); }
	| AbbreviatedRelativeLocationPath
	;
	
Step
  : AxisSpecifier NodeTest						{ $$ = pxpath_cat_paths(2, $1, $2); }
	| AxisSpecifier NodeTest Predicates  { $$ = pxpath_cat_paths(3, $1, $2, $3); }
	| AbbreviatedStep                   { $$ = PXP($1); }
	;
	
AxisSpecifier
  : AxisName DBLCOLON									{ $$ = pxpath_new_path(2, $1, $2); } 
	| AbbreviatedAxisSpecifier          { $$ = PXP($1); }
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
	
Predicates
  : Predicates Predicate               { $$ = pxpath_cat_paths(2, $1, $2); }
  | Predicate
	;
	
Predicate
  : LBRA PredicateExpr RBRA						 { $$ = PXPWRAP($1, $2, $3); }
	;
	
PredicateExpr
  : Expr
	;
	
AbbreviatedAbsoluteLocationPath
  : DBLSLASH RelativeLocationPath			{ $$ = PREP_OP($1, $2); }
	;
	
AbbreviatedRelativeLocationPath
  : RelativeLocationPath DBLSLASH Step	{ $$ = BIN_OP($1, $2, $3); }
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
  : LPAREN Argument RPAREN %dprec 2  { $$ = PXPWRAP($1, $2, $3);    }  
  | OrExpr						 %dprec 1
	;
PrimaryExpr
  : VariableReference 
	| LPAREN Expr RPAREN 				 	{ $$ = PXPWRAP($1, $2, $3);     }  
  | Literal                     { $$ = LIT($1);  }
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
  :	Argument COMMA ArgumentSet	 	  %dprec 2		{ $$ = $1; $1->next = $3; }
	| Argument												%dprec 1
	;
Argument
  : OptS Expr OptS		{ $$ = $2; }
	;
UnionExpr
  : PathExpr 
	| UnionExpr PIPE PathExpr							{ $$ = BIN_OP($1, $2, $3); }
	;
	
PathExpr
  : LocationPath 
	| FilterExpr                                    
	| FilterExpr SLASH RelativeLocationPath					{ $$ = BIN_OP($1, $2, $3); }
	| FilterExpr DBLSLASH RelativeLocationPath			{ $$ = BIN_OP($1, $2, $3); }
	;
	
FilterExpr
  : PrimaryExpr 
	| FilterExpr Predicates				{ $$ = pxpath_cat_paths(2, $1, $2); }
	;
	
OrExpr
  : AndExpr 
	| OrExpr XOR AndExpr						{ $$ = LIT_BIN_OP($1, $2, $3); }
	;
	
AndExpr
  : EqualityExpr 
	| AndExpr XAND EqualityExpr			{ $$ = LIT_BIN_OP($1, $2, $3); }
	;
	
EqualityExpr
  : RelationalExpr 
	| EqualityExpr EQ RelationalExpr				{ $$ = LIT_BIN_OP($1, $2, $3); }
	| EqualityExpr CXOPNE RelationalExpr		{ $$ = LIT_BIN_OP($1, $2, $3); }	
	;
	
RelationalExpr
  : AdditiveExpr                               %dprec 2
  | RelationalExpr OptS LT OptS AdditiveExpr   %dprec 3     { $$ = LIT_BIN_OP($1, $3, $5); }
  | RelationalExpr OptS GT OptS AdditiveExpr   %dprec 1     { $$ = LIT_BIN_OP($1, $3, $5); }
  | RelationalExpr OptS LTE OptS AdditiveExpr  %dprec 3     { $$ = LIT_BIN_OP($1, $3, $5); }
  | RelationalExpr OptS GTE OptS AdditiveExpr  %dprec 3     { $$ = LIT_BIN_OP($1, $3, $5); }
	;

AdditiveExpr
  : MultiplicativeExpr                             %dprec 3
	| AdditiveExpr OptS PLUS OptS MultiplicativeExpr %dprec 1 	{ $$ = LIT_BIN_OP($1, $3, $5); }
	| AdditiveExpr OptS DASH OptS MultiplicativeExpr %dprec 2 	{ $$ = LIT_BIN_OP($1, $3, $5); }
	;

MultiplicativeExpr
  : UnaryExpr                                                %dprec 2
  | MultiplicativeExpr OptS MultiplyOperator OptS UnaryExpr	 %dprec 2 { $$ = LIT_BIN_OP($1, $3, $5); }
  | MultiplicativeExpr OptS XDIV OptS UnaryExpr              %dprec 1 { $$ = LIT_BIN_OP($1, $3, $5); }
  | MultiplicativeExpr OptS XMOD OptS UnaryExpr              %dprec 2 { $$ = LIT_BIN_OP($1, $3, $5); }
	;

UnaryExpr
  : UnionExpr 
	| DASH UnaryExpr		{ $$ = PREP_OP($1, $2); }
	;
	
Literal
  : STRING
	;
Number
  : NUMBER                { $$ = LIT($1); }
	| NUMBER DOT						{ $$ = pxpath_new_literal(2, $1, $2); }
	| NUMBER DOT NUMBER			{ $$ = pxpath_new_literal(3, $1, $2, $3); }
	| DOT NUMBER						{ $$ = pxpath_new_literal(2, $1, $2); }
	;
	
MultiplyOperator
  : SPLAT
	;
	
VariableReference
  : DOLLAR QName				{	$$ = PREP_OP($1, $2); }
	;
	
NameTest
  : SPLAT               { $$ = PXP($1); }
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
	| UnprefixedName      { $$ = PXP($1); }
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
	| keyword
	;

selectors_group
	: attribute_extended_selector COMMA OptS selectors_group		{ $$ = pxpath_cat_paths(4, pxpath_new_literal(1, ".//"), $1, pxpath_new_literal(1, "|"), $4); }
	| attribute_extended_selector 	{ $$ = pxpath_cat_paths(2, pxpath_new_literal(1, ".//"), $1); }
  ;

attribute_extended_selector
	: selector
	| selector S AT NAME	{ $$ = pxpath_cat_paths(3, $1, PXP("/@"), PXP($4)); }
	;

selector
  : simple_selector_sequence combinator selector						{ $$ = pxpath_cat_paths(3, $1, PXP($2), $3); }
	| simple_selector_sequence
  ;
	
combinator
  : OptS PLUS OptS			{ $$ = "/following-sibling::*[1]/self::"; } 
	| OptS GT OptS				{ $$ = "/"; } 
	| OptS TILDE OptS		{ $$ = "/following-sibling::*/self::"; } 
	| S							{ $$ = "//"; }
  ;

simple_selector_sequence
	: simple_selector_anchor                        
	| possibly_empty_sequence LBRA	type_selector OptS CXOPHE OptS StringLike OptS RBRA                   { $$ = pxpath_cat_paths(10, $1, PXP("[@"), $3, PXP(" = "), $7, PXP(" or starts-with(@"), $3, PXP(", concat("), $7, PXP(", '-' ))]")); }
	| possibly_empty_sequence CXNOT LPAREN selectors_group RPAREN		                                      { $$ = pxpath_cat_paths(5, PXP("set-difference("), $1, PXP(", "), $4, PXP(")")); }
	| possibly_empty_sequence HASH Ident		                                                              { $$ = P4E($1, "[@id='", $3, "']"); }
	| possibly_empty_sequence DOT Ident		                                                                { $$ = P4E($1, "[contains(concat( ' ', @class, ' ' ), concat( ' ', '", $3, "', ' ' ))]"); }
	| possibly_empty_sequence LBRA	type_selector RBRA                                                    { $$ = P4E($1, "[@", $3, "]"); }
	| possibly_empty_sequence CXEQ LPAREN NumberLike RPAREN			                                          { $$ = P4E($1, "[position() = ", $3, "]"); }
	| possibly_empty_sequence CXGT LPAREN NumberLike RPAREN			                                          { $$ = P4E($1, "[position() > ", $3, "]"); }
	| possibly_empty_sequence CXLT LPAREN NumberLike RPAREN			                                          { $$ = P4E($1, "[position() < ", $3, "]"); }
	| possibly_empty_sequence CXCONTAINS	LPAREN StringLike RPAREN                                        { $$ = P4E($1, "[contains(., ", $4, ")]"); }
	| possibly_empty_sequence CXHAS LPAREN selectors_group RPAREN		                                      { $$ = P4E($1, "[", $3, "]"); }
	| possibly_empty_sequence CXNTHTYPE LPAREN NumberLike RPAREN		                                    	{ $$ = P4E($1, "[position() = ", $3, "]"); }
	| possibly_empty_sequence CXNTHLASTTYPE LPAREN NumberLike RPAREN			                                { $$ = P4E($1, "[position() = last() - ", $3, "]"); }
	| possibly_empty_sequence LBRA	type_selector OptS EQ OptS StringLike OptS RBRA                       { $$ = P6E($1, "[@", $3, " = ", $7, "]"); }
	| possibly_empty_sequence LBRA	type_selector OptS CXOPNE OptS StringLike OptS RBRA                   { $$ = P6E($1, "[@", $3, " != ", $7, "]"); }
	| possibly_empty_sequence LBRA	type_selector OptS CXOPSTARTEQ OptS StringLike OptS RBRA              { $$ = P6E($1, "[starts-with(@", $3, ", ", $7, ")]"); }
	| possibly_empty_sequence LBRA	type_selector OptS CXOPENDEQ OptS StringLike OptS RBRA                { $$ = P6E($1, "[ends-with(@", $3, ", ", $7, ")]"); }
	| possibly_empty_sequence LBRA	type_selector OptS CXOPCONTAINS OptS StringLike OptS RBRA             { $$ = P6E($1, "[contains(@", $3, ", ", $7, ")]"); }
	| possibly_empty_sequence LBRA	type_selector OptS CXOPCONTAINS2 OptS StringLike OptS RBRA            { $$ = P6E($1, "[contains(@", $3, ", ", $7, ")]"); }
	| possibly_empty_sequence CXNTHCH LPAREN NumberLike RPAREN 	                                          { $$ = P4O("*[", $4,  "]/self::", $1); }
	| possibly_empty_sequence CXNTHLASTCH LPAREN NumberLike RPAREN 	                                      { $$ = P4O("*[last() - ", $4, "]/self::", $1); }
	| possibly_empty_sequence CXFIRST	                                                                    { $$ = APPEND($1, "[1]"); }
	| possibly_empty_sequence CXLAST	                                                                  	{ $$ = APPEND($1, "[last()]"); }
	| possibly_empty_sequence CXEVEN		                                                                  { $$ = APPEND($1, "[position() % 2 = 0]"); }
	| possibly_empty_sequence CXODD		                                                                    { $$ = APPEND($1, "[position() % 2 = 1]"); }
	| possibly_empty_sequence CXCONTENT	LPAREN StringLike RPAREN	                                                                { $$ = P4E($1, "[translate(normalize-space(.), $nbsp, ' ')=", $4, "]"); }
	| possibly_empty_sequence CXHEADER		                                                                { $$ = APPEND($1, "[contains('h1 h2 h3 h4 h5 h6', lower-case(local-name()))]"); }
	| possibly_empty_sequence CXEMPTY		                                                                  { $$ = APPEND($1, "[not(node())]"); }
	| possibly_empty_sequence CXPARENT		                                                                { $$ = APPEND($1, "[node()]"); }
	| possibly_empty_sequence CXFIRSTTYPE                                                                 { $$ = APPEND($1, "[1]"); }
	| possibly_empty_sequence CXLASTTYPE                                                                  { $$ = APPEND($1, "[last()]"); }
	| possibly_empty_sequence CXONLYTYPE 	                                                                { $$ = APPEND($1, "[count()=1]"); }
	| possibly_empty_sequence CXINPUT 		                                                                { $$ = APPEND($1, "[lower-case(name())='input']"); }
	| possibly_empty_sequence CXENABLED 	                                                                { $$ = APPEND($1, "[lower-case(name())='input' and not(@disabled)]"); }
	| possibly_empty_sequence CXDISABLED 	                                                                { $$ = APPEND($1, "[lower-case(name())='input' and @disabled]"); }
	| possibly_empty_sequence CXCHECKED 	                                                                { $$ = APPEND($1, "[@checked]"); }
	| possibly_empty_sequence CXSELECTED 	                                                                { $$ = APPEND($1, "[@selected]"); }
	| possibly_empty_sequence CXFIRSTCH 	                                                                { $$ = PREPEND($1, "*[1]/self::"); }
	| possibly_empty_sequence CXLASTCH 	                                                                  { $$ = PREPEND($1, "*[last()]/self::"); }
	| possibly_empty_sequence CXONLYCH 		                                                                { $$ = PREPEND($1, "*[count()=1]/self::"); }
  | possibly_empty_sequence CXTEXT 			                                                                { $$ = INPUT_TYPE($1, text); }
	| possibly_empty_sequence CXPASSWORD 	                                                                { $$ = INPUT_TYPE($1, password); }
	| possibly_empty_sequence CXRADIO 		                                                                { $$ = INPUT_TYPE($1, radio); }
	| possibly_empty_sequence CXCHECKBOX 	                                                                { $$ = INPUT_TYPE($1, checkbox); }
	| possibly_empty_sequence CXSUBMIT 		                                                                { $$ = INPUT_TYPE($1, submit); }
	| possibly_empty_sequence CXIMAGE 		                                                                { $$ = INPUT_TYPE($1, image); }
	| possibly_empty_sequence CXRESET 		                                                                { $$ = INPUT_TYPE($1, reset); }
	| possibly_empty_sequence CXBUTTON 		                                                                { $$ = INPUT_TYPE($1, button); }
	| possibly_empty_sequence CXFILE 			                                                                { $$ = INPUT_TYPE($1, file); }
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
  : Ident  { $$ = $1; }
  ;

universal
  : namespace_prefix SPLAT { $$ = pxpath_cat_paths(3, $1, PXP(":"), PXP($2)); }
  | SPLAT                   { $$ = PXP($1); }
  ;

NumberLike
	: NUMBER                  { $$ = PXP($1); }
  ;

Ident
	: NAME                { $$ = PXP($1); }
	| BSLASHLIT						{ $$ = PXP($1 + 1); }
	| NAME Ident					{ $$ = pxpath_cat_paths(2, PXP($1), $2); }
	| BSLASHLIT Ident			{ $$ = pxpath_cat_paths(2, PXP($1 + 1), $2); }
	| keyword             { $$ = PXP($1); }
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

pxpathPtr myparse(char* string){
  // start_debugging();
  parsed_answer = NULL;
  prepare_parse(string);
  yyparse();
  cleanup_parse();
	return parsed_answer;
}

void answer(pxpathPtr a){
	parsed_answer = a;
}

void start_debugging(){
  yydebug = 1;
  return;
}   