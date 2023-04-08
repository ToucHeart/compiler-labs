#include "symbolTable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <string.h>
#include "treeNode.h"

char *mystrdup(const char *str)
{
    char *p = (char *)malloc(strlen(str) + 1);
    strcpy(p, str);
    return p;
}

static unsigned int hash(char *name)
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
Symbol *SymbolTable[HASH_TABLE_SIZE];

Symbol *serarch(char *name)
{
    unsigned key = hash(name);
}

bool insert(Symbol *s)
{
    unsigned key = hash(s->name);
    if (SymbolTable[key] == NULL)
    {
        SymbolTable[key] = s;
    }
    else
    {
    }
}

// 每个Def就是一条变量定义，它包括一个类型描述符Specifier以及一个DecList
void DefList(Node *deflist)
{
}

void FunDec(Node *f, Symbol *p)
{
}

void specifier(Node *node, Symbol *s)
{
}

FieldList *getStruct(Node *node)
{
}

// specifier
Type *getType(Node *node)
{
    Type *ret = (Type *)malloc(sizeof(Type));
    if (!strcmp(node->unitName, "TYPE"))
    {
        if (node->type == TOKEN_TYPE_INT)
        {
            ret->kind = BASIC;
            ret->t.basicType = INT;
        }
        else if (node->type == TOKEN_TYPE_FLOAT)
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

void Vardec(Node *node, Type *t) // node ==  ID | VarDec LB INT RB
{
    if (strcmp(node->unitName, "ID") == 0) // simple variable
    {
        Symbol *s = (Symbol *)malloc(sizeof(Symbol));
        s->name = mystrdup(node->val.str);
        s->next = NULL;
        s->type = t;
        insert(s);
    }
    else if (strcmp(node->unitName, "VarDec") == 0) // arrary
    {
    }
    else
    {
        assert(false);
    }
}

// int global1, global2;
void ExtDecList(Node *subtree, Type *t)
{
    Vardec(subtree->child, t);
    if (subtree->sibling != NULL)
    {
        Node *nextid = subtree->sibling->sibling; // nextid== ExtDecList
        ExtDecList(nextid->child, t);
    }
}

// 每个ExtDef表示一个全局变量、结构体或函数的定义
// ExtDef → Specifier ExtDecList SEMI
// ExtDef → Specifier SEMI
// ExtDef → Specifier FunDec CompSt
void Extdef(Node *extdef)
{
    Node *firstchild = extdef->child, *secondchild = firstchild->sibling, *thirdchild = secondchild->sibling;
    Type *t = getType(firstchild->child);
    if (strcmp(secondchild->unitName, "FunDec") == 0)
    {
    }
    else if (strcmp(secondchild->unitName, "ExtDecList") == 0)
    {
        ExtDecList(secondchild->child, t);
    }
    else if (strcmp(secondchild->unitName, "SEMI") == 0)
    {
    }
    else
    {
        assert(0);
    }
}

void ExdefList(Node *subtree) // subtree unitName == "ExtDefList"
{
    if (subtree == NULL)
        return;
    for (Node *q = subtree->child; q != NULL; q = q->sibling)
    {
        if (strcmp(q->unitName, "ExtDef") == 0)
        {
            Extdef(q);
        }
        else if (strcmp(q->unitName, "ExtDefList") == 0)
        {
            ExdefList(q);
        }
    }
}