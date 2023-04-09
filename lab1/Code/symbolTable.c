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

Symbol* serarch(char* name)
{
    unsigned key = hash(name);
}

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
                if (strcmp(p->name, name) == 0)
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

// 每个Def就是一条变量定义，它包括一个类型描述符Specifier以及一个DecList
void DefList(Node* deflist)
{
}




FieldList* getStruct(Node* node)
{
}

// specifier
Type* Specifier(Node* n)
{
    Type* ret = (Type*)malloc(sizeof(Type));
    Node* node = n->child;
    if (!strcmp(node->unitName, "TYPE"))
    {
        if (strcmp(node->val.str, "int") == 0)
        {
            ret->kind = BASIC;
            ret->t.basicType = INT;
        }
        else if (strcmp(node->val.str, "float") == 0)
        {
            ret->kind = BASIC;
            ret->t.basicType = FLOAT;
        }
    }
    else if (!strcmp(node->unitName, "StructSpecifier"))
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

char* Array(Node* node, Type* t) // VarDec LB INT RB
{
    if (strcmp(node->unitName, "ID") == 0)
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
        }
    }
    else if (strcmp(node->unitName, "VarDec") == 0)
    {
        Type* temp = (Type*)malloc(sizeof(Type));
        temp->kind = ARRAY;
        temp->t.array.elem = t;
        temp->t.array.size = node->sibling->sibling->val.int_val;
        Array(node->child, temp);
    }
    else
    {
        assert(false);
    }
}

void Vardec(Node* n, Type* t) // VarDec → ID | VarDec LB INT RB
{
    Node* node = n->child;
    if (strcmp(node->unitName, "ID") == 0) // simple variable
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
        }
    }
    else if (strcmp(node->unitName, "VarDec") == 0) // arrary
    {
        Array(node, t);
    }
    else
    {
        assert(false);
    }
}

// int global1, global2;
void ExtDecList(Node* subtree, Type* t) // subtree is firstchild of ExtDecList,== vardec
{
    Vardec(subtree->child, t);
    if (subtree->child->sibling != NULL)
    {
        Node* nextid = subtree->child->sibling->sibling; // nextid== ExtDecList
        ExtDecList(nextid, t);
    }
}

void CompSt(Node* node) {}

//ParamDec → Specifier VarDec
void ParamDec(Node* node, Symbol* s)
{
    Type* temp = Specifier(node->child);
    Vardec(node->child->sibling, temp);
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
        if (strcmp(thirdone->unitName, "RP") == 0)
        {
            s->type->t.function.argc = 0;
            s->type->t.function.params = NULL;
        }
        else if (strcmp(thirdone->unitName, "VarList") == 0)
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
    if (strcmp(secondchild->unitName, "FunDec") == 0) // function definition
    {
        FunDec(secondchild, t);
    }
    else if (strcmp(secondchild->unitName, "ExtDecList") == 0) // variable definition,including array
    {
        ExtDecList(secondchild, t);
    }
    else if (strcmp(secondchild->unitName, "SEMI") == 0) // struct definition
    {
    }
    else
    {
        assert(0);
    }
}

void ExtDefList(Node* subtree) // subtree unitName == "ExtDefList"
{
    if (subtree == NULL)
        return;
    for (Node* q = subtree->child; q != NULL; q = q->sibling)
    {
        if (strcmp(q->unitName, "ExtDef") == 0)
        {
            ExtDef(q);
        }
        else if (strcmp(q->unitName, "ExtDefList") == 0)
        {
            ExtDefList(q);
        }
    }
}
extern Node* root;
void analyseTree()
{
#if 1
    printf("analyseTree\n");
#endif
    if (root != NULL)
        ExtDefList(root->child);
}