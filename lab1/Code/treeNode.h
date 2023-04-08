#ifndef __NODE_H__
#define __NODE_H__
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>

typedef enum nodeType
{
    TOKEN_TYPE_INT,
    TOKEN_TYPE_FLOAT,
    TOKEN_TYPE_ID,
    TOKEN_TYPE_TYPE,
    TOKEN_TYPE_OTHER,
    NOT_TOKEN,
} NodeType;

typedef struct Node
{
    int lineNum;
    char *unitName; // int i;name ==i ; ID:func
    NodeType type;  // int,float,id
    union nodeVal
    {
        int int_val;
        float float_val;
        char *str;
    } val;
    struct Node *child;   // 左子女
    struct Node *sibling; // 右兄弟
} Node;

Node *getLexicalUnitNode(int num, char *name, NodeType t, char *val);
Node *getSyntaxUnitNode(int num, char *name, NodeType t, int argc, ...);
void freeNode(Node *subroot);
void freeMemory();
void printNode(Node *subroot, int depth);
void printTree();
#endif