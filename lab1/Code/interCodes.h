#ifndef __INTERCODES_H__
#define __INTERCODES_H__

typedef struct Operand *OperandPtr;
typedef struct InterCode *InterCodePtr;
typedef struct InterCodes *InterCodesPtr;
typedef struct InterCodesList *InterCodesListPtr;
typedef struct ArgNode *ArgNodePtr;
typedef struct ArgList *ArgListPtr;

#include "syntaxTree.h"
#include "symbolTable.h"

typedef enum RELOP_Kind
{
    RELOP_EQ,         //==
    RELOP_NEQ,        //!=
    RELOP_GREATER,    //>
    RELOP_LESS,       //<
    RELOP_GREATER_EQ, //>=
    RELOP_LESS_EQ,    //<=
} RELOP_Kind;

typedef enum OP_Kind
{
    OP_NONE,
    OP_VARIABLE,
    OP_TEMP,
    OP_IMM,
    OP_ADDRESS,
    OP_FUNCTION,
    OP_LABEL,
    OP_RELOP,
    OP_STRUCT_ARR_ID,
} OP_Kind;

typedef struct Operand
{
    OP_Kind kind;
    union
    {
        int value;
        char *name;
    } u;
    bool isArg;
    bool isBasicAddr;
} Operand;

typedef enum IR_Kind
{
    IR_NONE,
    IR_ASSIGN,
    IR_ADD,
    IR_SUB,
    IR_MUL,
    IR_DIV,
    IR_FUNCTION,
    IR_PARAM,
    IR_DEC,
    IR_RETURN,
    IR_LABEL,
    IR_GOTO,
    IR_IF_GOTO,
    IR_READ,
    IR_WRITE,
    IR_CALL,
    IR_ARG,
    IR_READ_ADDR,
    IR_WRITE_ADDR,
    IR_READ_WRITE_ADDR,
    IR_GET_ADDR,
} IR_Kind;

typedef struct InterCode
{
    IR_Kind kind;
    union
    {
        struct
        {
            OperandPtr varName;
            int size;
        } dec;
        struct
        {
            OperandPtr op;
        } oneop;
        struct
        {
            OperandPtr left, right;
        } assign;
        struct
        {
            OperandPtr result, left, right;
        } binop;
        struct
        {
            OperandPtr left, op, right, result;
        } ifgoto;
    } u;
} InterCode;

typedef struct InterCodes
{
    InterCodePtr code;
    InterCodesPtr prev, next;
} InterCodes;

typedef struct InterCodesList // 双向循环链表
{
    InterCodesPtr dummyHead;
    int tmpIndex;
    int labelIndex;
} InterCodesList;

typedef struct ArgNode
{
    OperandPtr arg;
    ArgNodePtr next;
} ArgNode;

typedef struct ArgList
{
    ArgNodePtr head;
} ArgList;

Symbol *translateExp(Node *node, OperandPtr place);
void translateCompSt(Node *node);
void printOp(FILE *output, OperandPtr op);
OperandPtr getOneOpOp(InterCodePtr code);
OperandPtr getAssLeft(InterCodePtr code);
OperandPtr getAssRight(InterCodePtr code);
OperandPtr getBinRes(InterCodePtr code);
OperandPtr getBinLeft(InterCodePtr code);
OperandPtr getBinRight(InterCodePtr code);
OperandPtr getGotoleft(InterCodePtr code);
OperandPtr getGotoOp(InterCodePtr code);
OperandPtr getGotoRight(InterCodePtr code);
OperandPtr getGotoResult(InterCodePtr code);
#endif