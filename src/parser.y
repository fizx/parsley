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

%token S
%token INCLUDES
%token DASHMATCH
%token PREFIXMATCH
%token SUFFIXMATCH
%token SUBSTRINGMATCH
%token IDENT
%token STRING
%token FUNCTION
%token NUMBER
%token HASH
%token PLUS
%token GREATER
%token COMMA
%token TILDE
%token NOT
%token ATKEYWORD
%token INVALID
%token PERCENTAGE
%token DIMENSION
%token CDO
%token CDC
%token URI
%token URI
%token UNICODE_RANGE


%%

    /* ======= START XPATH ========= */
			/*
								Root
								  : Argument {  printf("hello: %s\n", $1);    }
								  ;
								*/
LocationPath                                          
  : RelativeLocationPath                              
  | AbsoluteLocationPath                              
  ;                                                   
AbsoluteLocationPath                                  
  : SLASH                                             
  | SLASH RelativeLocationPath                        
  | AbbreviatedAbsoluteLocationPath                   
  ;                                                   
RelativeLocationPath                                  
  : Step                                              
  | RelativeLocationPath SLASH Step                   
  | AbbreviatedRelativeLocationPath                   
  ;                                                   
Step                                                  
  : AxisSpecifier NodeTest                            
  | AxisSpecifier NodeTest Predicate                  
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
  : AT                                                
  |                                                   
  ;                                                   
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
  : FunctionName LPAREN Arguments RPAREN              
  ;                                                   
Arguments                                             
  :                                                   
  | Argument                                          
  | Argument COMMA Arguments                          
  ;                                                   
Argument                                              
  : Expr                                              
  | CSSExpr                                           
  |                                                               
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
  : Digits                                            
  | Digits DOT                                        
  | Digits DOT Digits                                 
  |        DOT Digits                                 
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
  : NodeType  /* TODO: throw invalid */               
  | QName                                             
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
CSSExpr
  : selectors_group
  ;

optwhite
	: S
	|
	;

		/* CSS */
selectors_group
  : selector
	| selector COMMA optwhite selectors_group
  ;
selector
  : simple_selector_sequence
	| simple_selector_sequence combinator selector
  ;
combinator
  : PLUS optwhite
	| GREATER optwhite
	| TILDE optwhite
	| S
  ;
simple_selector_sequence
	: simple_selector_sequence_first
	| simple_selector_sequence simple_selector_sequence_following
	;
simple_selector_sequence_first
	: type_selector 
	| universal 
	| simple_selector_sequence_following
	;
simple_selector_sequence_following
	: HASH 
	| class 
	| attrib 
	| pseudo 
	| negation
	;
type_selector
  : element_name
	| namespace_prefix element_name
  ;
namespace_prefix
  : '|'
	| IDENT '|'
	| '*' '|'
  ;
element_name
  : IDENT
  ;
universal
  : namespace_prefix '*'
	| '*'
  ;
class
  : '.' IDENT
  ;

attrib
  : '[' attrib_inner ']'
  ;
optname
	: namespace_prefix
	|
	;
attrib_inner
	: optwhite optname IDENT optwhite optmisc
	;
optmisc
	: group_a optwhite group_b optwhite
	|
	;
group_a
	: PREFIXMATCH 
	| SUFFIXMATCH 
	| SUBSTRINGMATCH 
	| '=' 
	| INCLUDES 
	| DASHMATCH
	;
group_b
  : IDENT 
	| STRING
	;
pseudo
	: COLON IDENT
	| COLON COLON IDENT
	| COLON functional_pseudo
	| COLON COLON functional_pseudo
  ;
functional_pseudo
  : FUNCTION optwhite expressions rparen
  ;
expressions
	: expression
	| expression optwhite expressions
	;
expression
  : PLUS 
	| '-' 
	| DIMENSION 
	| NUMBER 
	| STRING 
	| IDENT
  ;
negation
  : NOT optwhite negation_arg optwhite RPAREN
  ;

negation_arg
  : type_selector 
	| universal 
	| HASH 
	| class 
	| attrib 
	| pseudo
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