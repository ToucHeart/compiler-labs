/*
 * this file is the definition of symbol table
 * and operations on it,including insert,search...
*/

#include "symbolTable.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

char* mystrdup(const char* str)
{
    char* p = (char*)malloc(strlen(str) + 1);
    strcpy(p, str);
    return p;
}

Type* newType()
{
    Type* t = (Type*)malloc(sizeof(Type));
    memset(t, 0x0, sizeof(Type));
    t->isRval = false;
    t->kind = NONE;
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

bool strEqual(const char* str1, const char* str2)
{
    return strcmp(str1, str2) == 0;
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

bool searchTableItem(char* name, Kind k)
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
            if (k == CANNOT_DUP && p->type->kind != FUNCTION || k == p->type->kind)
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

bool serarchStructItem(char* name, Symbol* structHead)
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
                if (k == CANNOT_DUP && p->type->kind != FUNCTION || k == p->type->kind)
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