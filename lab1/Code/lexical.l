%{
%}

%option yylineno

newline [\n\r]
ws [ \t]
STRUCT  struct
RETURN  return
IF  if
ELSE  else
WHILE  while
digit [0-9]
letter [_a-zA-Z]
lettersec [0-9_a-zA-Z]
TYPE int|float
SEMI  ";"
COMMA  ","
ASSIGNOP "="
RELOP  >|<|>=|<=|==|!=
PLUS  "+"
MINUS  "-"
STAR  "*"
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
ID {letter}{lettersec}*
INT 0|[1-9]+[0-9]*
FLOAT {digit}+"."{digit}+|{digit}*"."{digit}+[eE][+-]?{digit}+|{digit}+"."{digit}*[eE][+-]?{digit}+
%%
{letter}+ {printf("letter %s\n",yytext);}
{digit}+ {printf("digit %s\n",yytext);}
{ws}+ { printf("ws %s\n",yytext);}
{ASSIGNOP} {printf("ASSIGNOP %s\n",yytext);}
{SEMI} {printf("SEMI %s\n",yytext);}
{newline} {printf("newline %s\n",yytext);}
{LP} {printf("LP %s\n",yytext);}
{RP} {printf("RP %s\n",yytext);}
{LC} {printf("LC %s\n",yytext);}
{RC} {printf("RC %s\n",yytext);}
{PLUS} { printf("PLUS %s\n",yytext); }
{MINUS} { printf("SUB %s\n",yytext); }
{AND} { printf("AND %s\n",yytext); }
{OR} { printf("OR %s\n",yytext); }
. {printf("Error type A at Line %d: Mysterious characters \'%s\'\n",yylineno, yytext);}
%%