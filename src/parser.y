%{
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "kstring.h"

#define YYSTYPE char *

int yylex (void);
void yyerror (char const *);

void start_debugging(void);

int yyparse(void);
void myparse(char*);
  
%}

%glr-parser
%token_table
%debug

%token LPAREN
%token DESC
%token DIV
%token TEXT
%token DASH
%token ANCESTORSELF
%token AT
%token S
%token PIPE
%token GTE
%token NODE
%token PI
%token DSTR
%token NCName
%token NS
%token PRESIB
%token PLUS
%token DESCSELF
%token SLASH
%token LTE
%token ATTR
%token COMMA
%token RBRA
%token SPLAT
%token SSTR
%token PARENT
%token GT
%token SELF
%token FOLLOW
%token OR
%token NE
%token CHILD
%token CSSExpr
%token LT
%token AND
%token COMMENT
%token ANCESTOR
%token LBRA
%token DOT
%token RPAREN
%token DOLLAR
%token EQ
%token Digits
%token COLON
%token PRE
%token MOD
%token FOLLOWSIB

%%

    /* ======= START XPATH ========= */

Root
  : Argument {  printf("hello: %s\n", $1);    }
  ;
LocationPath 
  : RelativeLocationPath                              { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | AbsoluteLocationPath                              { $$ = strdup($1);  printf("hi: %s\n", $1); }
  ;
AbsoluteLocationPath 
  : SLASH                                             { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | SLASH RelativeLocationPath                        { $$ = astrcat($1, $2); }
  | AbbreviatedAbsoluteLocationPath                   { $$ = strdup($1);  printf("hi: %s\n", $1); }
  ;
RelativeLocationPath 
  : Step                                              { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | RelativeLocationPath SLASH Step                   { $$ = astrcat3($1, $2, $3); }
  | AbbreviatedRelativeLocationPath                   { $$ = strdup($1);  printf("hi: %s\n", $1); }
  ;
Step 
  : AxisSpecifier NodeTest                            { $$ = astrcat($1, $2); }
  | AxisSpecifier NodeTest Predicate                  { $$ = astrcat3($1, $2, $3); }
  | AbbreviatedStep                                   { $$ = strdup($1);  printf("hi: %s\n", $1); }
  ;
AxisSpecifier 
  : AxisName COLON COLON                              { $$ = astrcat3($1, $2, $3); }
  | AbbreviatedAxisSpecifier                          { $$ = strdup($1);  printf("hi: %s\n", $1); }
  ;
AxisName 
	: ANCESTOR                                          { $$ = strdup($1);  printf("hi: %s\n", $1); }
	| ANCESTORSELF                                      { $$ = strdup($1);  printf("hi: %s\n", $1); }
	| ATTR	                                            { $$ = strdup($1);  printf("hi: %s\n", $1); }
	| CHILD	                                            { $$ = strdup($1);  printf("hi: %s\n", $1); }
	| DESC	                                            { $$ = strdup($1);  printf("hi: %s\n", $1); }
	| DESCSELF	                                        { $$ = strdup($1);  printf("hi: %s\n", $1); }
	| FOLLOW	                                          { $$ = strdup($1);  printf("hi: %s\n", $1); }
	| FOLLOWSIB	                                        { $$ = strdup($1);  printf("hi: %s\n", $1); }
	| NS	                                              { $$ = strdup($1);  printf("hi: %s\n", $1); }
	| PARENT	                                          { $$ = strdup($1);  printf("hi: %s\n", $1); }
	| PRE	                                              { $$ = strdup($1);  printf("hi: %s\n", $1); }
	| PRESIB	                                          { $$ = strdup($1);  printf("hi: %s\n", $1); }
	| SELF	                                            { $$ = strdup($1);  printf("hi: %s\n", $1); }
	;
NodeTest 
  : NameTest                                          { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | NodeType LPAREN RPAREN                            { $$ = astrcat3($1, $2, $3); }
  | PI LPAREN Literal RPAREN                          { $$ = astrcat3($1, $2, $3); }
  ;
Predicate 
  : LBRA PredicateExpr RBRA                           { $$ = astrcat3($1, $2, $3); }
  ;
PredicateExpr 
  : Expr                                              { $$ = strdup($1);  printf("hi: %s\n", $1); }
  ;
AbbreviatedAbsoluteLocationPath 
  : SLASH SLASH RelativeLocationPath                  { $$ = astrcat3($1, $2, $3); }
  ;
AbbreviatedRelativeLocationPath 
  : RelativeLocationPath SLASH SLASH Step             { $$ = astrcat4($1, $2, $3, $4); }
  ;
AbbreviatedStep 
  : DOT                                               { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | DOT DOT                                           { $$ = astrcat($1, $2); }
  ;
AbbreviatedAxisSpecifier 
  : AT                                                { $$ = strdup($1);  printf("hi: %s\n", $1); }
  |                                                   
  ;
Expr 
  : OrExpr                                            { $$ = strdup($1);  printf("hi: %s\n", $1); }
  ;                                                   
PrimaryExpr 
  : VariableReference                                 { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | LPAREN Expr RPAREN                                { $$ = astrcat3($1, $2, $3); }
  | Literal                                           { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | Number                                            { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | FunctionCall                                      { $$ = strdup($1);  printf("hi: %s\n", $1); }
  ;
FunctionCall 
  : FunctionName LPAREN Arguments RPAREN              { $$ = astrcat4($1, $2, $3, $4); }
  ;
Arguments
  :                                                   
  | Argument                                          { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | Argument COMMA Arguments                          { $$ = astrcat3($1, $2, $3); }
  ;
Argument 
  : Expr                                              { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | CSSExpr                                           { $$ = strdup($1);  printf("hi: %s\n", $1); }
  |                                                   { $$ = "/* empty args */"; }                                              
  ;
UnionExpr
  : PathExpr                                          { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | UnionExpr PIPE PathExpr                           { $$ = astrcat3($1, $2, $3); }
  ;
PathExpr 
  : LocationPath                                      { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | FilterExpr                                        { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | FilterExpr SLASH RelativeLocationPath             { $$ = astrcat3($1, $2, $3); }
  | FilterExpr SLASH SLASH RelativeLocationPath       { $$ = astrcat3($1, $2, $3); }
  ;
FilterExpr 
  : PrimaryExpr                                       { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | FilterExpr Predicate                              { $$ = astrcat($1, $2); }    
  ;
OrExpr 
  : AndExpr                                           { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | OrExpr OR AndExpr                                 { $$ = strdup($1);  printf("hi: %s\n", $1); }
  ;
AndExpr
  : EqualityExpr                                      { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | AndExpr AND EqualityExpr                          { $$ = astrcat3($1, $2, $3); }
  ;
EqualityExpr 
  : RelationalExpr                                    { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | EqualityExpr EQ RelationalExpr                    { $$ = astrcat3($1, $2, $3); }
  | EqualityExpr NE RelationalExpr                    { $$ = astrcat3($1, $2, $3); }
  ;
RelationalExpr 
  : AdditiveExpr                                      { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | RelationalExpr LT AdditiveExpr                    { $$ = astrcat3($1, $2, $3); }
  | RelationalExpr GT AdditiveExpr                    { $$ = astrcat3($1, $2, $3); }
  | RelationalExpr LTE AdditiveExpr                   { $$ = astrcat3($1, $2, $3); }
  | RelationalExpr GTE AdditiveExpr                   { $$ = astrcat3($1, $2, $3); }
  ;
AdditiveExpr 
  : MultiplicativeExpr                                { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | AdditiveExpr PLUS MultiplicativeExpr              { $$ = astrcat3($1, $2, $3); }
  | AdditiveExpr DASH MultiplicativeExpr              { $$ = astrcat3($1, $2, $3); }
  ;
MultiplicativeExpr 
  : UnaryExpr                                         { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | MultiplicativeExpr MultiplyOperator UnaryExpr     { $$ = astrcat3($1, $2, $3); }
  | MultiplicativeExpr DIV UnaryExpr                  { $$ = astrcat3($1, $2, $3); }
  | MultiplicativeExpr MOD UnaryExpr                  { $$ = astrcat3($1, $2, $3); }
  ;
UnaryExpr 
  : UnionExpr                                         { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | DASH UnaryExpr                                    { $$ = astrcat($1, $2); }
  ;
ExprToken 
  : LPAREN                                            { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | RPAREN                                            { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | LBRA                                              { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | RBRA                                              { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | DOT                                               { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | DOT DOT                                           { $$ = astrcat($1, $2); }
  | AT                                                { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | COMMA                                             { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | COLON COLON                                       { $$ = astrcat($1, $2); }
  | NameTest                                          { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | NodeType                                          { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | Operator                                          { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | FunctionName                                      { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | AxisName                                          { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | Literal                                           { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | Number                                            { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | VariableReference                                 { $$ = strdup($1);  printf("hi: %s\n", $1); }
  ;
Literal 
  : DSTR                                              { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | SSTR                                              { $$ = strdup($1);  printf("hi: %s\n", $1); }
  ;
Number 
  : Digits                                            { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | Digits DOT                                        { $$ = astrcat($1, $2); }
  | Digits DOT Digits                                 { $$ = astrcat3($1, $2, $3); }
  |        DOT Digits                                 { $$ = astrcat($1, $2); }
  ;
Operator 
  : OperatorName                                      { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | MultiplyOperator                                  { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | SLASH                                             { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | SLASH SLASH                                       { $$ = astrcat($1, $2); }
  | PIPE                                              { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | PLUS                                              { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | DASH                                              { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | EQ                                                { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | NE                                                { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | LT                                                { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | LTE                                               { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | GT                                                { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | GTE                                               { $$ = strdup($1);  printf("hi: %s\n", $1); }
  ;
OperatorName 
  : AND                                               { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | OR                                                { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | MOD                                               { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | DIV                                               { $$ = strdup($1);  printf("hi: %s\n", $1); }
  ;
MultiplyOperator 
  : SPLAT                                             { $$ = strdup($1);  printf("hi: %s\n", $1); }
  ;
FunctionName 
  : NodeType  /* TODO: throw invalid */               { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | QName                                             { $$ = strdup($1);  printf("hi: %s\n", $1); }
  ;
VariableReference 
  : DOLLAR QName                                      { $$ = strdup($1);  printf("hi: %s\n", $1); }
  ;
NameTest 
  : SPLAT                                             { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | NCName COLON SPLAT                                { $$ = astrcat3($1, $2, $3); }
  | QName                                             { $$ = strdup($1);  printf("hi: %s\n", $1); }
  ;
NodeType 
  : COMMENT                                           { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | TEXT                                              { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | PI                                                { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | NODE                                              { $$ = strdup($1);  printf("hi: %s\n", $1); }
  ;
ExprWhitespace 
  : S                                                 { $$ = strdup($1);  printf("hi: %s\n", $1); }
  ;
QName
	: PrefixedName                                      { $$ = strdup($1);  printf("hi: %s\n", $1); }
  | UnprefixedName                                    { $$ = strdup($1);  printf("hi: %s\n", $1); }
  ;
PrefixedName
  : Prefix COLON LocalPart                            { $$ = astrcat3($1, $2, $3); }
  ;
UnprefixedName
  : LocalPart                                         { $$ = strdup($1);  printf("hi: %s\n", $1); }
  ;
Prefix
  : NCName                                            { $$ = strdup($1);  printf("hi: %s\n", $1); }
  ;
LocalPart
  : NCName                                            { $$ = strdup($1);  printf("hi: %s\n", $1); }
  ;
  
  
%%

void yyerror (const char * s) {
  printf("%s\n", s);
  exit(1);
}

void start_debugging(){
  yydebug = 1;
  return;
}