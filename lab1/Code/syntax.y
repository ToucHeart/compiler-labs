%locations
%{
#define YYERROR_VERBOSE 1
#include "lex.yy.c"
extern Node *root;
extern bool hasError;
int yyerror(const char* msg) 
{
    hasError=true;
    printf("Error type B at Line %d: %s\n",yylineno,msg);
}
%}

%union{
    Node* nodePtr; 
}

// terminals/tokens
%token <nodePtr> INT
%token <nodePtr> FLOAT
%token <nodePtr> ID
%token <nodePtr> IF
%token <nodePtr> ELSE
%token <nodePtr> WHILE
%token <nodePtr> RETURN
%token <nodePtr> STRUCT
%token <nodePtr> TYPE
%token <nodePtr> COMMA
%token <nodePtr> DOT
%token <nodePtr> SEMI
%token <nodePtr> RELOP
%token <nodePtr> ASSIGNOP
%token <nodePtr> PLUS MINUS STAR DIV
%token <nodePtr> AND OR NOT 
%token <nodePtr> LP RP LB RB LC RC

//not terminals,
%type <nodePtr> Program ExtDefList ExtDef ExtDecList   //  High-level Definitions
%type <nodePtr> Specifier StructSpecifier OptTag Tag   //  Specifiers
%type <nodePtr> VarDec FunDec VarList ParamDec         //  Declarators
%type <nodePtr> CompSt StmtList Stmt                   //  Statements
%type <nodePtr> DefList Def Dec DecList                //  Local Definitions
%type <nodePtr> Exp Args                               //  Expressions

//越往下优先级越高
%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT
%left DOT
%left LB RB
%left LP RP
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%
Program : ExtDefList  {root = getSyntaxUnitNode(@$.first_line, "Program",NOT_TOKEN,1, $1);}
;
ExtDefList  :  ExtDef  ExtDefList   {$$ = getSyntaxUnitNode(@$.first_line, "ExtDefList",NOT_TOKEN,2, $1,$2);}
|                                   {$$ = NULL;}
;
ExtDef : Specifier ExtDecList SEMI  {$$ = getSyntaxUnitNode(@$.first_line, "ExtDef",NOT_TOKEN,3, $1,$2,$3);}
| Specifier SEMI                    {$$ = getSyntaxUnitNode(@$.first_line, "ExtDef",NOT_TOKEN,2, $1,$2);}
| Specifier FunDec CompSt           {$$ = getSyntaxUnitNode(@$.first_line, "ExtDef",NOT_TOKEN,3, $1,$2,$3);}
|  error SEMI                       {}
| Specifier FunDec error SEMI       {}
;
ExtDecList :    VarDec              {$$ = getSyntaxUnitNode(@$.first_line, "ExtDecList",NOT_TOKEN,1, $1);}
| VarDec COMMA ExtDecList           {$$ = getSyntaxUnitNode(@$.first_line, "ExtDecList",NOT_TOKEN,3, $1,$2,$3);}
;
Specifier : TYPE                    {$$ = getSyntaxUnitNode(@$.first_line, "Specifier",NOT_TOKEN,1,$1);}
| StructSpecifier                   {$$ = getSyntaxUnitNode(@$.first_line, "Specifier",NOT_TOKEN,1,$1);}
;
StructSpecifier : STRUCT OptTag LC DefList RC  {$$ = getSyntaxUnitNode(@$.first_line,"StructSpecifier",NOT_TOKEN,5,$1,$2,$3,$4,$5);}
| STRUCT Tag                                   {$$ = getSyntaxUnitNode(@$.first_line,"StructSpecifier",NOT_TOKEN,2,$1,$2);}
;
OptTag : ID {$$ = getSyntaxUnitNode(@$.first_line,"OptTag",NOT_TOKEN,1,$1);}
|           {$$ = NULL;}
;
Tag : ID    {$$ = getSyntaxUnitNode(@$.first_line,"Tag",NOT_TOKEN,1,$1);}
;
VarDec : ID                 {$$ = getSyntaxUnitNode(@$.first_line,"VarDec",NOT_TOKEN,1,$1);}
| VarDec LB INT RB          {$$ = getSyntaxUnitNode(@$.first_line,"VarDec",NOT_TOKEN,4,$1,$2,$3,$4);}
|  error RB         {}
;
FunDec : ID LP VarList RP   {$$ = getSyntaxUnitNode(@$.first_line,"FunDec",NOT_TOKEN,4,$1,$2,$3,$4);}
| ID LP RP                  {$$ = getSyntaxUnitNode(@$.first_line,"FunDec",NOT_TOKEN,3,$1,$2,$3);}
|  error RP      {}
;
VarList : ParamDec COMMA VarList  {$$ = getSyntaxUnitNode(@$.first_line,"VarList",NOT_TOKEN,3,$1,$2,$3);}
| ParamDec                        {$$ = getSyntaxUnitNode(@$.first_line,"VarList",NOT_TOKEN,1,$1);}
;
ParamDec : Specifier VarDec   {$$ = getSyntaxUnitNode(@$.first_line,"ParamDec",NOT_TOKEN,2,$1,$2);}
;
CompSt : LC DefList StmtList RC  {$$ = getSyntaxUnitNode(@$.first_line,"CompSt",NOT_TOKEN,4,$1,$2,$3,$4);}
| error RC                       {}
;
StmtList : Stmt StmtList         {$$ = getSyntaxUnitNode(@$.first_line,"StmtList",NOT_TOKEN,2,$1,$2);}
|                                {$$ = NULL;}
;
Stmt : Exp SEMI                  {$$ = getSyntaxUnitNode(@$.first_line,"Stmt",NOT_TOKEN,2,$1,$2);}
| CompSt                         {$$ = getSyntaxUnitNode(@$.first_line,"Stmt",NOT_TOKEN,1,$1);}
| RETURN Exp SEMI                {$$ = getSyntaxUnitNode(@$.first_line,"Stmt",NOT_TOKEN,3,$1,$2,$3);}
| IF LP Exp RP Stmt %prec LOWER_THAN_ELSE  {$$ = getSyntaxUnitNode(@$.first_line,"Stmt",NOT_TOKEN,5,$1,$2,$3,$4,$5);}
| IF LP Exp RP Stmt ELSE Stmt    {$$ = getSyntaxUnitNode(@$.first_line,"Stmt",NOT_TOKEN,7,$1,$2,$3,$4,$5,$6,$7);}
| WHILE LP Exp RP Stmt           {$$ = getSyntaxUnitNode(@$.first_line,"Stmt",NOT_TOKEN,5,$1,$2,$3,$4,$5);}
| error SEMI                     {}
;
DefList : Def DefList            {$$ = getSyntaxUnitNode(@$.first_line,"DefList",NOT_TOKEN,2,$1,$2);}
|                                {$$ = NULL;}
;
Def : Specifier DecList SEMI     {$$ = getSyntaxUnitNode(@$.first_line,"Def",NOT_TOKEN,3,$1,$2,$3);}
;
DecList : Dec                    {$$ = getSyntaxUnitNode(@$.first_line,"DecList",NOT_TOKEN,1,$1);}
| Dec COMMA DecList              {$$ = getSyntaxUnitNode(@$.first_line,"DecList",NOT_TOKEN,3,$1,$2,$3);}
;
Dec : VarDec                     {$$ = getSyntaxUnitNode(@$.first_line,"Dec",NOT_TOKEN,1,$1);}
| VarDec ASSIGNOP Exp            {$$ = getSyntaxUnitNode(@$.first_line,"Dec",NOT_TOKEN,3,$1,$2,$3);}
;
Exp : Exp ASSIGNOP Exp           {$$ = getSyntaxUnitNode(@$.first_line,"Exp",NOT_TOKEN,3,$1,$2,$3);}
| Exp AND Exp                    {$$ = getSyntaxUnitNode(@$.first_line,"Exp",NOT_TOKEN,3,$1,$2,$3);}
| Exp OR Exp                     {$$ = getSyntaxUnitNode(@$.first_line,"Exp",NOT_TOKEN,3,$1,$2,$3);}
| Exp RELOP Exp                  {$$ = getSyntaxUnitNode(@$.first_line,"Exp",NOT_TOKEN,3,$1,$2,$3);}
| Exp PLUS Exp                    {$$ = getSyntaxUnitNode(@$.first_line,"Exp",NOT_TOKEN,3,$1,$2,$3);}
| Exp MINUS Exp                    {$$ = getSyntaxUnitNode(@$.first_line,"Exp",NOT_TOKEN,3,$1,$2,$3);}
| Exp STAR Exp                    {$$ = getSyntaxUnitNode(@$.first_line,"Exp",NOT_TOKEN,3,$1,$2,$3);}
| Exp DIV Exp                    {$$ = getSyntaxUnitNode(@$.first_line,"Exp",NOT_TOKEN,3,$1,$2,$3);}
| LP Exp RP                      {$$ = getSyntaxUnitNode(@$.first_line,"Exp",NOT_TOKEN,3,$1,$2,$3);}           
| MINUS Exp                        {$$ = getSyntaxUnitNode(@$.first_line,"Exp",NOT_TOKEN,2,$1,$2);}
| NOT Exp                        {$$ = getSyntaxUnitNode(@$.first_line,"Exp",NOT_TOKEN,2,$1,$2);}       
| ID LP Args RP                  {$$ = getSyntaxUnitNode(@$.first_line,"Exp",NOT_TOKEN,4,$1,$2,$3,$4);}   
| ID LP RP                       {$$ = getSyntaxUnitNode(@$.first_line,"Exp",NOT_TOKEN,3,$1,$2,$3);}               
| Exp LB Exp RB                  {$$ = getSyntaxUnitNode(@$.first_line,"Exp",NOT_TOKEN,4,$1,$2,$3,$4);}       
| Exp DOT ID                     {$$ = getSyntaxUnitNode(@$.first_line,"Exp",NOT_TOKEN,3,$1,$2,$3);}   
| ID                             {$$ = getSyntaxUnitNode(@$.first_line,"Exp",NOT_TOKEN,1,$1);}   
| INT                            {$$ = getSyntaxUnitNode(@$.first_line,"Exp",NOT_TOKEN,1,$1);}           
| FLOAT                          {$$ = getSyntaxUnitNode(@$.first_line,"Exp",NOT_TOKEN,1,$1);}
| Exp LB Exp error RB            {}
;
Args : Exp COMMA Args            {$$ = getSyntaxUnitNode(@$.first_line,"Args",NOT_TOKEN,3,$1,$2,$3);}
| Exp                            {$$ = getSyntaxUnitNode(@$.first_line,"Args",NOT_TOKEN,1,$1);}
;
%%
