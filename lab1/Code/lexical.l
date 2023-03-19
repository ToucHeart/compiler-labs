%{
    #include "syntax.tab.h"
%}

%option yylineno

NEWLINE [\n\r]+
WS [ \t]+
STRUCT  struct
RETURN  return
IF  if
ELSE  else
WHILE  while
DIGIT [0-9]
LETTER [_a-zA-Z]
LETTERSEC [0-9_a-zA-Z]
RELOP 	>|<|>=|<=|==|!=
TYPE int|float
SEMI  ";"
COMMA  ","
ASSIGNOP "="
ADD  "+"
SUB  "-"
MUL  "*"
DIV  "/"
AND "&&"
OR "||"
NOT "!"
DOT "."
LP "("
RP ")"
LB "["
RB "]"
LC "{"
RC "}"
ID {LETTER}{LETTERSEC}*
INT 0|[1-9]+[0-9]*
FLOAT {DIGIT}+"."{DIGIT}+|{DIGIT}*"."{DIGIT}+[eE][+-]?{DIGIT}+|{DIGIT}+"."{DIGIT}*[eE][+-]?{DIGIT}+
%%
{ID} {return ID;}
{INT} {yylval.type_int = atoi(yytext); return INT;}
{FLOAT} {yylval.type_float = atof(yytext); return FLOAT;}
{ADD} {return ADD;}
{SUB} {return SUB;}
{MUL} {return MUL;}
{DIV} {return DIV;}
{AND} {return AND;}
{OR} {return OR; }
{WS} {return WS;}
{RELOP} {return RELOP;}
{ASSIGNOP} {return ASSIGNOP;}
{SEMI} {return SEMI;}
{NEWLINE} {}
{COMMA} {return COMMA;}
{DOT} {return DOT;}
{LB} {return LB;}
{RB} {return RB;}
{LP} {return LP;}
{RP} {return RP;}
{LC} {return LC;}
{RC} {return RC;}
. {printf("Error type A at Line %d: Mysterious characters \'%s\'\n",yylineno, yytext);}
%%