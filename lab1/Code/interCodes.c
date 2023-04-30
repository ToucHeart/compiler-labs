#include"interCodes.h"
#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>
#include<string.h>
#include"syntaxTree.h"
#include<stdarg.h>

#define DEBUG 1

extern Node* root;
static InterCodesList list;
extern char* mystrdup(const char* str);

OperandPtr newOperand(int kind, int val, char* name)
{
    OperandPtr op = (OperandPtr)malloc(sizeof(Operand));
    op->kind = kind;
    op->u.value = val;
    op->u.name = name;
    return op;
}

InterCodePtr newInterCode(int kind, ...)
{
    InterCodePtr p = (InterCodePtr)malloc(sizeof(InterCode));
    p->kind = kind;
    va_list ap;
    va_start(ap, kind);
    switch (kind)
    {
    case IR_FUNCTION:
    p->u.oneop.op = va_arg(ap, OperandPtr);
    break;

    default:
    assert(0);
    break;
    }
    return p;
}

InterCodesPtr newInterCodes(InterCodePtr code)
{
    InterCodesPtr p = (InterCodesPtr)malloc(sizeof(InterCodes));
    p->code = code;
    p->next = p->prev = NULL;
    return p;
}

void initList()
{
    list.dummyHead = newInterCodes(NULL);
    list.dummyHead->next = list.dummyHead;
    list.dummyHead->prev = list.dummyHead;
    list.labelIndex = 1;
    list.tmpIndex = 1;
    list.varIndex = 1;
}

void addInterCodes(InterCodesPtr p)
{
    InterCodesPtr head = list.dummyHead;
    if (head->next == head)
    {
        head->next = p;
        head->prev = p;
        p->next = head;
        p->prev = head;
    }
    else
    {
        head->prev->next = p;
        p->prev = head->prev;
        head->prev = p;
        p->next = head;
    }
}

// FunDec -> ID LP VarList RP
//         | ID LP RP
void translateFunDec(Node* node)//函数名的标识符以及由一对圆括号括起来的一个形参列表
{
    Node* firstchild = node->child;
    OperandPtr op = newOperand(OP_FUNCTION, 0, mystrdup(firstchild->val.str));
    InterCodePtr code = newInterCode(IR_FUNCTION, op);
    InterCodesPtr codes = newInterCodes(code);
    addInterCodes(codes);
}

void translateCompSt(Node* node)
{

}

void translateExtdef(Node* node)
{
    //不需要处理全局变量和结构体定义,没有全局变量,只需要翻译函数
    Node* secondchild = node->child->sibling;
    if (strEqual(secondchild->unitName, "FunDec")) // function definition
    {
        translateFunDec(secondchild);
        translateCompSt(secondchild->sibling);
    }
}

void translateExtdefList(Node* node)
{
    for (Node* q = node->child; q != NULL; q = q->sibling)
    {
        if (strEqual(q->unitName, "ExtDef"))
        {
            translateExtdef(q);
        }
        else if (strEqual(q->unitName, "ExtDefList"))
        {
            translateExtdefList(q);
        }
    }
}

void genInterCodes()
{
    initList(&list);
    if (root != NULL)
    {
        translateExtdefList(root->child);
    }
}

void printOp(FILE* output, OperandPtr op)
{
    switch (op->kind)
    {
    case OP_CONSTANT:
    fprintf(output, "#%d", op->u.value);
    break;
    case OP_VARIABLE:
    case OP_ADDRESS:
    case OP_LABEL:
    case OP_FUNCTION:
    case OP_RELOP:
    fprintf(output, "%s", op->u.name);
    break;
    }
}

void printInterCode(FILE* output, InterCodePtr p)
{
    switch (p->kind)
    {
    case IR_FUNCTION:
    fprintf(output, "FUNCTION ");
    printOp(output, p->u.oneop.op);
    fprintf(output, " :");
    break;
    default:
    assert(0);
    break;
    }
    fprintf(output, "\n");
}

void printInterCodes(FILE* output)
{
    InterCodesPtr p = list.dummyHead->next;
    while (p != list.dummyHead)
    {
        printInterCode(output, p->code);
        p = p->next;
    }
}