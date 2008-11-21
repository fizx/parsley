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

int yyparse(void);
void myparse(char*);
  
%}

%glr-parser
%token_table

%token OTHER

%%

    /* ======= START XPATH ========= */

LocationPath 
  : RelativeLocationPath 
  | AbsoluteLocationPath
  ;
AbsoluteLocationPath 
  : SLASH RelativeLocationPath?
  | AbbreviatedAbsoluteLocationPath
  ;
RelativeLocationPath 
  : Step 
  | RelativeLocationPath SLASH Step
  | AbbreviatedRelativeLocationPath
  ;
Step 
  : AxisSpecifier NodeTest Predicate* 
  | AbbreviatedStep
  ;
AxisSpecifier 
  : AxisName COLON COLON 
  | AbbreviatedAxisSpecifier
  ;
AxisName 
	: ANCESTOR      
	| ANCESTORSELF  
	| ATTR	        
	| CHILD	        
	| DESC	        
	| DESCSELF	    
	| FOLLOW	      
	| FOLLOWSIB	    
	| NS	          
	| PARENT	      
	| PRE	          
	| PRESIB	      
	| SELF	        
	;
NodeTest 
  : NameTest 
  | NodeType LPAREN RPAREN
  | PI LPAREN Literal RPAREN
  ;
Predicate 
  : LBRA PredicateExpr RBRA
  ;
PredicateExpr 
  : Expr
  ;
AbbreviatedAbsoluteLocationPath 
  : SLASH SLASH RelativeLocationPath
  ;
AbbreviatedRelativeLocationPath 
  : RelativeLocationPath SLASH SLASH Step
  ;
AbbreviatedStep 
  : DOT 
  | DOT DOT
  ;
AbbreviatedAxisSpecifier 
  : AT?
Expr 
  : OrExpr
  ;
PrimaryExpr 
  : VariableReference 
  | LPAREN Expr RPAREN 
  | Literal
  | Number 
  | FunctionCall
  ;
FunctionCall 
  : FunctionName LPAREN ( Argument ( COMMA Argument )* )? RPAREN
  ;
Argument 
  : Expr
  | CSSExpr
  ;
UnionExpr
  : PathExpr 
  | UnionExpr PIPE PathExpr
  ;
PathExpr 
  : LocationPath 
  | FilterExpr
  | FilterExpr SLASH RelativeLocationPath
  | FilterExpr SLASH SLASH RelativeLocationPath
  ;
FilterExpr 
  : PrimaryExpr 
  | FilterExpr Predicate
  ;
OrExpr 
  : AndExpr 
  | OrExpr OR AndExpr
  ;
AndExpr
  : EqualityExpr 
  | AndExpr AND EqualityExpr
  ;
EqualityExpr 
  : RelationalExpr 
  | EqualityExpr EQ RelationalExpr
  | EqualityExpr NE RelationalExpr
  ;
RelationalExpr 
  : AdditiveExpr
  | RelationalExpr LT AdditiveExpr
  | RelationalExpr GT AdditiveExpr
  | RelationalExpr LTE AdditiveExpr
  | RelationalExpr GTE AdditiveExpr
  ;
AdditiveExpr 
  : MultiplicativeExpr
  | AdditiveExpr PLUS MultiplicativeExpr
  | AdditiveExpr DASH MultiplicativeExpr
  ;
MultiplicativeExpr 
  : UnaryExpr
  | MultiplicativeExpr MultiplyOperator UnaryExpr
  | MultiplicativeExpr DIV UnaryExpr
  | MultiplicativeExpr MOD UnaryExpr
  ;
UnaryExpr 
  : UnionExpr
  | DASH UnaryExpr
  ;
ExprToken 
  : LPAREN
  | RPAREN
  | LBRA
  | RBRA
  | DOT
  | DOT DOT
  | AT
  | COMMA
  | COLON COLON
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
  : DSTR 
  | SSTR
  ;
Number 
  : Digits (DOT Digits?)? 
  | DOT Digits
  ;
Digits 
  : DIGIT+
  ;
Operator 
  : OperatorName 
  | MultiplyOperator 
  | SLASH 
  | SLASH SLASH 
  | PIPE
  | PLUS 
  | DASH
  | EQ
  | NE
  | LT
  | LTE
  | GT
  | GTE
  ;
OperatorName 
  : AND 
  | OR 
  | MOD 
  | DIV
  ;
MultiplyOperator 
  : SPLAT
  ;
FunctionName 
  : QName - NodeType
  ;
VariableReference 
  : DOLLAR QName
  ;
NameTest 
  : SPLAT 
  | NCName COLON SPLAT 
  | QName
  ;
NodeType 
  : COMMENT
  | TEXT
  | PI
  | NODE
  ;
ExprWhitespace 
  : S
  ;
QName
	: PrefixedName
  | UnprefixedName
  ;
PrefixedName
  : Prefix COLON LocalPart
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
  
  
%%

void myparse(char* string){
  prepare_parse(string);
  yyparse();
  cleanup_parse();
}

void yyerror (const char * s) {
  printf("%s\n", s);
  exit(1);
}