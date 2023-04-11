#ifndef __SYMBOL_TABLE__
#define __SYMBOL_TABLE__

#include "syntaxTree.h"
#define HASH_TABLE_SIZE 0x3fff
typedef enum
{
    VAR,
    BASIC,
    ARRAY,
    STRUCTURE,
    FUNCTION,
    NONE,
}Kind;

typedef struct Type
{
    Kind kind;
    bool isLeftVal;
    union
    {
        // 基本类型
        enum
        {
            INT,
            FLOAT
        } basicType;
        // 数组类型信息包括元素类型与数组大小构成
        struct
        {
            struct Type* elem;
            int size;
        } array;
        // 结构体类型信息是一个链表
        struct Symbol* structure;
        struct
        {
            int argc;
            struct Parameter* params;
            struct Type* returnType;
        } function;
    } t;
} Type;

// 符号类型
typedef struct Symbol
{
    char* name;
    Type* type;
    struct Symbol* next; // same hash code next symbol, linked from hash table
} Symbol;

// 函数参数信息
typedef struct Parameter
{
    Type* t;
    struct Parameter* next;
} Parameter;

Symbol* StructSpecifier(Node* node);
Type* Specifier(Node* n);
void DefList(Node* node, Symbol* structinfo);
void CompSt(Node* node, Symbol* funcSym);
#endif