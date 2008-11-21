%{
#include <stdio.h>
#include "y.tab.h"

YY_BUFFER_STATE mybuffer;

void prepare_parse(char* msg) {
  mybuffer = yy_scan_string(msg);  
}

void cleanup_parse() {
  yy_delete_buffer(mybuffer);  
}
 
%}
              
DSTR          '"' [^"]+ '"'
SSTR          "'" [^']+ "'"
DIGIT         [0-9]
SLASH         '/'
PIPE          '|'
PLUS          '+'
DASH          '-'
EQ            '='
NE            '!='
LT            '<'
GT            '>'
GTE           '>='
LTE           '<='
AND           'and'
OR            'or'
MOD           'mod'
DIV           'div'
SPLAT         '*'
LPAREN        '('
RPAREN        ')'
LBRA          '['
RBRA          ']'
DOT           '.'
AT            '@'
COLON         ':'
DOLLAR        '$'
COMMA         ','
COMMENT       'comment'
TEXT          'text' 
PI            'processing-instruction' 
NODE          'node'
ANCESTOR      'ancestor' 
ANCESTORSELF  'ancestor-or-self' 
ATTR	        'attribute' 
CHILD	        'child' 
DESC	        'descendant' 
DESCSELF	    'descendant-or-self' 
FOLLOW	      'following'
FOLLOWSIB	    'following-sibling' 
NS	          'namespace' 
PARENT	      'parent' 
PRE	          'preceding' 
PRESIB	      'preceding-sibling' 
SELF	        'self'




OTHER       (.|\n)

%%