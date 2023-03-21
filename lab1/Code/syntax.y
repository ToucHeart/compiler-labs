%locations
%{
#include "lex.yy.c"
#include <stdio.h>
%}

%union{
    Node* node; 
}

// tokens

%token <node> INT
%token <node> FLOAT
%token <node> ID
%token <node> TYPE
%token <node> COMMA
%token <node> DOT
%token <node> SEMI
%token <node> RELOP
%token <node> ASSIGNOP
%token <node> ADD SUB MUL DIV
%token <node> AND OR NOT 
%token <node> LP RP LB RB LC RC
%token <node> IF
%token <node> ELSE
%token <node> WHILE
%token <node> STRUCT
%token <node> RETURN

// non-terminals
%type <node> Program ExtDefList ExtDef ExtDecList   //  High-level Definitions
%type <node> Specifier StructSpecifier OptTag Tag   //  Specifiers
%type <node> VarDec FunDec VarList ParamDec         //  Declarators
%type <node> CompSt StmtList Stmt                   //  Statements
%type <node> DefList Def Dec DecList                //  Local Definitions
%type <node> Exp Args                               //  Expressions

%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left ADD SUB
%left MUL DIV
%right NOT
%left DOT
%left LB RB
%left LP RP
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%
Program : ExtDefList  {}
;
ExtDefList  :  ExtDef   {}
| ExtDefList            {}
;
ExtDef : Specifier ExtDecList SEMI  {}
| Specifier SEMI                    {}
| Specifier FunDec CompSt           {}
;
ExtDecList :    VarDec              {}
| VarDec COMMA ExtDecList           {}
;
Specifier : TYPE                    {}
| StructSpecifier                   {}
;
StructSpecifier : STRUCT OptTag LC DefList RC  {}
| STRUCT Tag                                   {}
;
OptTag : ID {}
|           {}
;
Tag : ID    {}
;
VarDec : ID  {}
| VarDec LB INT RB  {}
;
FunDec : ID LP VarList RP  {}
| ID LP RP     {}
;
VarList : ParamDec COMMA VarList  {}
| ParamDec   {}
;
ParamDec : Specifier VarDec   {}
;
CompSt : LC DefList StmtList RC  {}
StmtList : Stmt StmtList         {}
|  {}
;
Stmt : Exp SEMI                  {}
| CompSt                         {}
| RETURN Exp SEMI                {}
| IF LP Exp RP Stmt              {}
| IF LP Exp RP Stmt ELSE Stmt    {}
| WHILE LP Exp RP Stmt           {}
;
DefList : Def DefList            {}
|  {}
;
Def : Specifier DecList SEMI     {}
;
DecList : Dec                    {}
| Dec COMMA DecList              {}
;
Dec : VarDec                     {}
| VarDec ASSIGNOP Exp            {}
;
Exp : Exp ASSIGNOP Exp           {}
| Exp AND Exp                    {}
| Exp OR Exp                     {}
| Exp RELOP Exp                  {}
| Exp ADD Exp                   {}
| Exp SUB Exp                  {}
| Exp MUL Exp                   {}
| Exp DIV Exp                    {}
| LP Exp RP                      {}           
| SUB Exp                      {}
| NOT Exp                        {}       
| ID LP Args RP                  {}   
| ID LP RP                       {}               
| Exp LB Exp RB                  {}       
| Exp DOT ID                     {}   
| ID                             {}   
| INT                            {}           
| FLOAT                          {}
;
Args : Exp COMMA Args            {}
| Exp                            {}
;
%%