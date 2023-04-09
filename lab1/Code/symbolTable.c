#include "symbolTable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <string.h>
#include "syntaxTree.h"
#include<stdarg.h>

char* mystrdup(const char* str)
{
    char* p = (char*)malloc(strlen(str) + 1);
    strcpy(p, str);
    return p;
}

bool strEqual(const char* str1, const char* str2)
{
    return strcmp(str1, str2) == 0;
}

static void printSemanticError(int errType, int lineNum, char* message, int num, ...)
{
    printf("Error type %d at Line %d: %s", errType, lineNum, message);
    va_list valist;
    va_start(valist, num);
    for (int i = 0; i < num; i++)
    {
        char* str = va_arg(valist, char*);
        printf("%s", str);
    }
    printf("\n");
    va_end(valist);
}

static unsigned int hash(char* name)
{
    unsigned int val = 0, i;
    for (; *name; ++name)
    {
        val = (val << 2) + *name;
        if (i = val & ~HASH_TABLE_SIZE)
            val = (val ^ (i >> 12)) & HASH_TABLE_SIZE;
    }
    return val;
}

// global symbol table
Symbol* SymbolTable[HASH_TABLE_SIZE + 1];

bool contains(char* name, Kind k)
{
    unsigned key = hash(name);
    if (SymbolTable[key] == NULL)
    {
        return false;
    }
    else
    {
        Symbol* p = SymbolTable[key];
        while (p != NULL)
        {
            if (k == VAR && p->type->kind != FUNCTION || k == p->type->kind)
            {
                if (strEqual(p->name, name))
                {
                    return true;
                }
            }
            p = p->next;
        }
    }
    return false;
}

void insert(Symbol* s)
{
    unsigned key = hash(s->name);
    if (SymbolTable[key] == NULL)
    {
        SymbolTable[key] = s;
    }
    else
    {
        s->next = SymbolTable[key];
        SymbolTable[key] = s;
    }
}






FieldList* getStruct(Node* node)
{
}

// specifier
Type* Specifier(Node* n)
{
    Type* ret = (Type*)malloc(sizeof(Type));
    Node* node = n->child;
    if (strEqual(node->unitName, "TYPE"))
    {
        if (strEqual(node->val.str, "int"))
        {
            ret->kind = BASIC;
            ret->t.basicType = INT;
        }
        else if (strEqual(node->val.str, "float"))
        {
            ret->kind = BASIC;
            ret->t.basicType = FLOAT;
        }
    }
    else if (strEqual(node->unitName, "StructSpecifier"))
    {
        ret->kind = STRUCTURE;
        ret->t.structure = getStruct(node->child);
    }
    else
    {
        assert(false);
    }
    return ret;
}

Symbol* Array(Node* node, Type* t) // VarDec LB INT RB
{
    if (strEqual(node->unitName, "ID"))
    {
        char* name = node->val.str;
        if (contains(name, VAR))
        {
            printSemanticError(3, node->lineNum, "Redefined variable \"", 2, name, "\".");
        }
        else
        {
            Symbol* s = (Symbol*)malloc(sizeof(Symbol));
            s->name = mystrdup(name);
            s->type = t;
            s->next = NULL;
            insert(s);
            return s;
        }
    }
    else if (strEqual(node->unitName, "VarDec"))
    {
        Type* temp = (Type*)malloc(sizeof(Type));
        temp->kind = ARRAY;
        temp->t.array.elem = t;
        temp->t.array.size = node->sibling->sibling->val.int_val;
        return Array(node->child, temp);
    }
    else
    {
        assert(false);
    }
    return NULL;
}

Symbol* VarDec(Node* n, Type* t) // VarDec → ID | VarDec LB INT RB
{
    Node* node = n->child;
    if (strEqual(node->unitName, "ID")) // simple variable
    {
        char* name = node->val.str;
        if (contains(name, VAR))
        {
            printSemanticError(3, node->lineNum, "Redefined variable \"", 2, name, "\".");
        }
        else
        {
            Symbol* s = (Symbol*)malloc(sizeof(Symbol));
            s->name = mystrdup(node->val.str);
            s->next = NULL;
            s->type = t;
            insert(s);
            return s;
        }
    }
    else if (strEqual(node->unitName, "VarDec")) // arrary
    {
        return Array(node, t);
    }
    else
    {
        assert(false);
    }
    return NULL;
}

// int global1, global2;
void ExtDecList(Node* subtree, Type* t) // subtree is firstchild of ExtDecList,== vardec
{
    VarDec(subtree->child, t);
    if (subtree->child->sibling != NULL)
    {
        Node* nextid = subtree->child->sibling->sibling; // nextid== ExtDecList
        ExtDecList(nextid, t);
    }
}
/*
StmtList → Stmt StmtList | 空
Stmt → Exp SEMI
| CompSt
| RETURN Exp SEMI
| IF LP Exp RP Stmt
| IF LP Exp RP Stmt ELSE Stmt
| WHILE LP Exp RP Stmt
*/
void StmtList(Node* node)
{

}

void Exp(Node* node)
{

}
// Dec → VarDec | VarDec ASSIGNOP Exp
void Dec(Node* node, Type* type)
{
    Node* first = node->child;
    Symbol* s = VarDec(first, type);//TODO:暂存符号,如果不为NULL的话后面会用到
    if (first->sibling != NULL)
    {
        Exp(first->sibling->sibling);//TODO: check Assignment legal
    }
}
// DecList → Dec | Dec COMMA DecList
void DecList(Node* node, Type* type)
{
    assert(strEqual(node->unitName, "DecList"));
    Node* first = node->child;
    Dec(first, type);
    if (first->sibling != NULL)
    {
        DecList(first->sibling->sibling, type);
    }
}
// 每个Def就是一条变量定义，它包括一个类型描述符Specifier以及一个DecList
// Def → Specifier DecList SEMI
void Def(Node* node)
{
    assert(strEqual(node->unitName, "Def"));
    Node* first = node->child, * second = first->sibling;
    Type* temp = Specifier(first);
    DecList(second, temp);
}
// DefList → Def DefList | 空
void DefList(Node* node)
{
    assert(strEqual(node->unitName, "DefList"));
    Node* def = node->child;
    Def(def);
    if (def->sibling != NULL)
    {
        DefList(def->sibling);
    }
}

//CompSt → LC DefList StmtList RC
void CompSt(Node* node)
{
    Node* secondone = node->child->sibling;
    Node* thirdone = NULL;
    if (strEqual(secondone->unitName, "DefList"))
    {
        DefList(secondone);
        thirdone = secondone->sibling;
    }
    else if (strEqual(secondone->unitName, "StmtList"))
    {
        thirdone = secondone;
    }
    if (thirdone != NULL && strEqual(thirdone->unitName, "StmtList"))
    {
        StmtList(thirdone);
    }
}

//ParamDec → Specifier VarDec
void ParamDec(Node* node, Symbol* s)
{
    Type* temp = Specifier(node->child);
    VarDec(node->child->sibling, temp);
    Parameter* p = s->type->t.function.params;
    if (p->t == NULL)
    {
        p->t = temp;
    }
    else
    {
        while (p->next != NULL)
        {
            p = p->next;
        }
        Parameter* m = (Parameter*)malloc(sizeof(Parameter));
        m->t = temp;
        m->next = NULL;
        p->next = m;
    }
}
//VarList → ParamDec COMMA VarList
//        | ParamDec
void VarList(Node* node, Symbol* s)
{
    ParamDec(node->child, s);
    if (node->child->sibling != NULL)
    {
        VarList(node->child->sibling->sibling, s);
    }
}
//FunDec → ID LP VarList RP 
//       | ID LP RP
void FunDec(Node* node, Type* retType)
{
    Node* idnode = node->child;
    char* name = idnode->val.str;
    if (contains(name, FUNCTION))
    {
        printSemanticError(4, node->lineNum, "Redefined function \"", 2, name, "\".");
    }
    else
    {
        Symbol* s = (Symbol*)malloc(sizeof(Symbol));
        s->name = mystrdup(name);
        s->next = NULL;
        s->type = (Type*)malloc(sizeof(Type));
        s->type->kind = FUNCTION;
        s->type->t.function.returnType = retType;
        Node* thirdone = idnode->sibling->sibling;
        if (strEqual(thirdone->unitName, "RP"))
        {
            s->type->t.function.argc = 0;
            s->type->t.function.params = NULL;
        }
        else if (strEqual(thirdone->unitName, "VarList"))
        {
            s->type->t.function.params = (Parameter*)malloc(sizeof(Parameter));
            Parameter* p = s->type->t.function.params;
            p->next = NULL;
            p->t = NULL;
            VarList(thirdone, s);
        }
        CompSt(node->sibling);
        insert(s);
    }
}

// 每个ExtDef表示一个全局变量、结构体或函数的定义
// ExtDef → Specifier ExtDecList SEMI
// ExtDef → Specifier SEMI
// ExtDef → Specifier FunDec CompSt
void ExtDef(Node* extdef)
{
    Node* firstchild = extdef->child, * secondchild = firstchild->sibling;
    Type* t = Specifier(firstchild);
    if (strEqual(secondchild->unitName, "FunDec")) // function definition
    {
        FunDec(secondchild, t);
    }
    else if (strEqual(secondchild->unitName, "ExtDecList")) // variable definition,including array
    {
        ExtDecList(secondchild, t);
    }
    else if (strEqual(secondchild->unitName, "SEMI")) // struct definition
    {
    }
    else
    {
        assert(0);
    }
}

void ExtDefList(Node* subtree) // subtree unitName == "ExtDefList"
{
    for (Node* q = subtree->child; q != NULL; q = q->sibling)
    {
        if (strEqual(q->unitName, "ExtDef"))
        {
            ExtDef(q);
        }
        else if (strEqual(q->unitName, "ExtDefList"))
        {
            ExtDefList(q);
        }
    }
}
extern Node* root;
void analyseTree()
{
    if (root != NULL)
        ExtDefList(root->child);
}