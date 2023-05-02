/*
 * this file is the definition of symbol table
 * and operations on it,including insert,search...
*/

#include "symbolTable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include"help.h"

Type* newType()
{
    Type* t = (Type*)malloc(sizeof(Type));
    memset(t, 0x0, sizeof(Type));
    t->isRval = false;
    t->kind = TYPE_NONE;
    return t;
}

Symbol* newSymbol(char* name)
{
    Symbol* sym = (Symbol*)malloc(sizeof(Symbol));
    if (name != NULL)
        sym->name = mystrdup(name);
    sym->type = newType();
    sym->next = NULL;
    memset(sym->type, 0, sizeof(Type));
    return sym;
}

Parameter* newParameter()
{
    Parameter* p = (Parameter*)malloc(sizeof(Parameter));
    p->next = NULL;
    p->t = NULL;
    return p;
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

bool hasTableItem(char* name, Kind k)
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
            if (k == TYPE_CANNOT_DUP && p->type->kind != TYPE_FUNCTION || k == p->type->kind)
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

bool hasStructItem(char* name, Symbol* structHead)
{
    Symbol* head = structHead;
    while (head != NULL)
    {
        if (strEqual(name, head->name))
            return true;
        head = head->next;
    }
    return false;
}

void insertStructItem(Symbol* structinfo, Symbol* s)
{
    Symbol* p = structinfo->type->t.structure;
    if (p == NULL)
    {
        structinfo->type->t.structure = s;
    }
    else
    {
        while (p->next != NULL)
        {
            p = p->next;
        }
        p->next = s;
        s->next = NULL;
    }
}

void insertTableItem(Symbol* s)
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

Symbol* getTableSymbol(char* name, Kind k)
{
    unsigned key = hash(name);
    if (SymbolTable[key] == NULL)
    {
        return NULL;
    }
    else
    {
        Symbol* p = SymbolTable[key];
        while (p != NULL)
        {
            if (strEqual(p->name, name))
            {
                if (k == TYPE_CANNOT_DUP && p->type->kind != TYPE_FUNCTION || k == p->type->kind)//变量名和函数名可以重复
                    return p;
            }
            p = p->next;
        }
    }
    return NULL;
}

Symbol* getStructItem(char* name, Symbol* structHead)
{
    Symbol* head = structHead;
    while (head != NULL)
    {
        if (strEqual(name, head->name))
            return head;
        head = head->next;
    }
    return NULL;
}

int getTypeSize(Type* t)
{
    switch (t->kind)
    {
    case TYPE_BASIC:
    return 4;
    break;
    case TYPE_ARRAY:
    return t->t.array.size * getTypeSize(t->t.array.elem);
    break;
    case TYPE_STRUCTURE:
    {
        int size = 0;
        Symbol* s = t->t.structure;
        while (s != NULL)
        {
            size += getTypeSize(s->type);
            s = s->next;
        }
        return size;
    }
    default:
    assert(0);
    break;
    }
    return 0;
}