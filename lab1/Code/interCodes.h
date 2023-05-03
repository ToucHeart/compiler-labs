#ifndef __INTERCODES_H__
#define __INTERCODES_H__

typedef struct Operand* OperandPtr;
typedef struct InterCode* InterCodePtr;
typedef struct InterCodes* InterCodesPtr;
typedef struct InterCodesList* InterCodesListPtr;

typedef struct Operand
{
    enum
    {
        OP_NONE,
        OP_VARIABLE,
        OP_CONSTANT,
        OP_ADDRESS,
        OP_FUNCTION,
        OP_LABEL,
        OP_RELOP,
    }kind;
    union
    {
        int value;
        char* name;
    } u;
}Operand;

typedef struct InterCode
{
    enum
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
    } kind;
    union
    {
        struct
        {
            OperandPtr varName;
            int size;
        }dec;
        struct
        {
            OperandPtr op;
        }oneop;
        struct
        {
            OperandPtr  left, right;
        }assign;
        struct
        {
            OperandPtr result, left, right;
        }binop;
    } u;
}InterCode;

typedef struct InterCodes
{
    InterCodePtr code;
    InterCodesPtr prev, next;
}InterCodes;

typedef struct InterCodesList//双向循环链表
{
    InterCodesPtr dummyHead;
    int varIndex;
    int tmpIndex;
    int labelIndex;
}InterCodesList;

#endif