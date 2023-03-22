#include "Node.h"
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
/*
    int lineNum;
    char *name;    // int i;name ==i ; ID:func
    NodeType type; // int,float,id
    union nodeVal
    {
        int int_val;
        float float_val;
        char *str;
    } val;
    struct Node *child; // 左子女
    struct Node *sibling; // 右兄弟

    typedef enum nodeType
    {
        TOKEN_TYPE_INT,
        TOKEN_TYPE_FLOAT,
        TOKEN_TYPE_ID,
        TOKEN_TYPE_OTHER,
        NOT_TOKEN,
    } NodeType;
*/
Node *root = NULL;

Node *getSyntaxUnitNode(int num, char *name, NodeType t, int argc, ...)
{

    Node *p = (Node *)malloc(sizeof(Node));
    p->lineNum = num;
    p->unitName = strdup(name);
    p->type = t;
    va_list arglist;
    va_start(arglist, argc);
    p->child = va_arg(arglist, Node *);
    Node *q = p->child;
    for (int i = 1; i < argc; ++i)
    {
        q->sibling = va_arg(arglist, Node *);
        q = q->sibling;
    }
    va_end(arglist);
    return p;
}

Node *getLexicalUnitNode(int num, char *name, NodeType t, char *val)
{
    Node *p = (Node *)malloc(sizeof(Node));
    p->lineNum = num;
    p->unitName = strdup(name);
    p->type = t;
    p->sibling = p->child = NULL;
    switch (t)
    {
    case TOKEN_TYPE_INT:
        p->val.int_val = atoi(val);
        break;
    case TOKEN_TYPE_FLOAT:
        p->val.float_val = atof(val);
        break;
    case TOKEN_TYPE_ID:
        p->val.str = strdup(val);
    case TOKEN_TYPE_OTHER:
        break;
    default:
        assert(0);
        break;
    }
    return p;
}
void printTree()
{
    printNode(0);
}
void printNode(int depth)
{
}
void freeNode(Node *subroot)
{
    Node *p = subroot->child;
    while (p != NULL)
    {
        Node *temp = p;
        p = p->sibling;
        freeNode(temp);
    }
    free(subroot->unitName);
    if (subroot->type == TOKEN_TYPE_ID)
        free(subroot->val.str);
    free(subroot);
}

void freeMemory()
{
    if (root != NULL)
        freeNode(root);
}