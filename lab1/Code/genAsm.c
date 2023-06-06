#include <stdio.h>
#include "interCodes.h"
#include "help.h"
#define REG_NUM 32

extern InterCodesList list;

static const char *regs[REG_NUM] = {
    "$zero",
    "$at",
    "$v0",
    "$v1",
    "$a0",
    "$a1",
    "$a2",
    "$a3",
    "$t0",
    "$t1",
    "$t2",
    "$t3",
    "$t4",
    "$t5",
    "$t6",
    "$t7",
    "$s0",
    "$s1",
    "$s2",
    "$s3",
    "$s4",
    "$s5",
    "$s6",
    "$s7",
    "$t8",
    "$t9",
    "$k0",
    "$k1",
    "$gp",
    "$sp",
    "$fp",
    "$ra",
};

void printStartCode(FILE *output)
{
    FILE *startcode = fopen("partcode.s", "r");
    int c;
    while ((c = fgetc(startcode)) != EOF)
    {
        fputc(c, output);
    }
    fputc('\n', output);
}

const char *getReg()
{
    static int i = 0;
    i = (i + 1) % 32;
    return regs[i];
}

void printRet(FILE *output, OperandPtr op)
{
    // move $v0, reg(x)
    // jr $ra
    // if (p->u.oneop.op->kind == OP_ADDRESS)
    //     fprintf(output, "*");
    // printOp(output, p->u.oneop.op);

    fprintf(output, "move $v0, %s\n", getReg());
    fprintf(output, "jr $ra\n");
}

/*
x := #k  li reg(x), k
x := y   move reg(x), reg(y)
*/
void printAssign(FILE *output, OperandPtr left, OperandPtr right)
{
    if (left == NULL)
        return;
    if (right->kind == OP_CONSTANT)
    {
        fprintf(output, "li %s, %d\n", getReg(), right->u.value);
    }
    else if (right->kind == OP_ADDRESS)
    {
        fprintf(output, "lw %s, 0(%s)", getReg(), getReg());
    }
    else if (left->kind == OP_ADDRESS)
    {
    }
    else
    {
        fprintf(output, "move %s, %s\n", getReg(), getReg());
    }
}

/*
x := y + #k     addi reg(x), reg(y), k
x := y + z      add reg(x), reg(y), reg(z)
x := y - #k     addi reg(x), reg(y), -k
x := y – z      sub reg(x), reg(y), reg(z)
x := y * z2     mul reg(x), reg(y), reg(z)
x := y / z      div reg(y), reg(z)
                mflo reg(x)
*/
void printCal(FILE *output, OperandPtr res, OperandPtr left, OperandPtr right, int kind)
{
    switch (kind)
    {
    case IR_ADD:
        if (right->kind == OP_CONSTANT)
        {
            fprintf(output, "addi %s, %s, %d\n", getReg(), getReg(), right->u.value);
        }
        else
        {
            fprintf(output, "add %s, %s, %s\n", getReg(), getReg(), getReg());
        }
        break;
    case IR_SUB:
        if (right->kind == OP_CONSTANT)
        {
            fprintf(output, "addi %s, %s, -%d\n", getReg(), getReg(), right->u.value);
        }
        else
        {
            fprintf(output, "sub %s, %s, %s\n", getReg(), getReg(), getReg());
        }
        break;
    case IR_MUL:
        fprintf(output, "mul %s, %s, %s\n", getReg(), getReg(), getReg());
        break;
    case IR_DIV:
        fprintf(output, "div %s, %s\n", getReg(), getReg());
        fprintf(output, "mflo %s\n", getReg());
        break;
    }
}

/*
IF x == y GOTO z    beq reg(x), reg(y), z
IF x != y GOTO z    bne reg(x), reg(y), z
IF x > y GOTO z     bgt reg(x), reg(y), z
IF x < y GOTO z     blt reg(x), reg(y), z
IF x >= y GOTO z    bge reg(x), reg(y), z
IF x <= y GOTO z    ble reg(x), reg(y), z
*/
void printGoto(FILE *output, OperandPtr left, OperandPtr op, OperandPtr right, OperandPtr result)
{
    if (strEqual(op->u.name, "=="))
    {
        fprintf(output, "beq %s, %s, %s\n", getReg(), getReg(), result->u.value);
    }
    else if (strEqual(op->u.name, "!="))
    {
    }
    else if (strEqual(op->u.name, ">"))
    {
    }
    else if (strEqual(op->u.name, "<"))
    {
    }
    else if (strEqual(op->u.name, ">="))
    {
    }
    else if (strEqual(op->u.name, "<="))
    {
    }
}

void printasm(FILE *output, InterCodePtr p)
{
    switch (p->kind)
    {
    case IR_FUNCTION:
        printOp(output, getOneOpOp(p));
        fprintf(output, ":\n");
        break;

    case IR_PARAM:
        fprintf(output, "PARAM ");
        printOp(output, getOneOpOp(p));
        break;

    case IR_DEC:
        fprintf(output, "DEC %s %d", p->u.dec.varName->u.name, p->u.dec.size);
        break;

    case IR_ASSIGN:
        printAssign(output, getAssLeft(p), getAssRight(p));
        break;

    case IR_GET_ADDR: // 将地址赋给临时变量
        printOp(output, p->u.assign.left);
        fprintf(output, " := ");
        if (p->u.assign.right->isArg == false)
            fprintf(output, "&");
        printOp(output, p->u.assign.right);
        break;

    case IR_READ_ADDR:
        printAssign(output, getAssLeft(p), getAssRight(p));
        break;

    case IR_WRITE_ADDR:
        printAssign(output, getAssLeft(p), getAssRight(p));
        break;

    case IR_READ_WRITE_ADDR:
        fprintf(output, "*");
        printOp(output, p->u.assign.left);
        fprintf(output, " := *");
        printOp(output, p->u.assign.right);
        break;

    case IR_ADD:
    case IR_SUB:
    case IR_MUL:
    case IR_DIV:
        printCal(output, getBinRes(p), getBinLeft(p), getBinRight(p), p->kind);
        break;

    case IR_RETURN:
        printRet(output, getOneOpOp(p));
        break;

    case IR_GOTO:
        fprintf(output, "j ");
        printOp(output, getOneOpOp(p));
        break;

    case IR_LABEL:
        printOp(output, getOneOpOp(p));
        fprintf(output, ":");
        break;

    case IR_IF_GOTO:
        printGoto(output, getGotoleft(p), getGotoOp(p), getGotoRight(p), getGotoResult(p));
        break;

    case IR_READ:
        fprintf(output, "READ ");
        printOp(output, p->u.oneop.op);
        break;

    case IR_WRITE:
        fprintf(output, "WRITE ");
        if (p->u.oneop.op->kind == OP_ADDRESS)
            fprintf(output, "*");
        printOp(output, p->u.oneop.op);
        break;
    case IR_CALL:
        if (p->u.assign.left->kind == OP_ADDRESS)
            fprintf(output, "*");
        printOp(output, p->u.assign.left);
        fprintf(output, " := CALL ");
        printOp(output, p->u.assign.right);
        break;

    case IR_ARG:
        fprintf(output, "ARG ");
        if (p->u.oneop.op->kind == OP_STRUCT_ARR_ID)
            fprintf(output, "&");
        else if (p->u.oneop.op->kind == OP_ADDRESS && p->u.oneop.op->isBasicAddr)
            fprintf(output, "*");
        printOp(output, p->u.oneop.op);
        break;
    default:
        fprintf(stderr, RED "code type is %d\n" NORMAL, p->kind);
        assert(0);
        break;
    }
    fprintf(output, "\n");
}

void printAsm(FILE *output)
{
    printStartCode(output);
    InterCodesPtr p = list.dummyHead->next;
    while (p != list.dummyHead)
    {
        printasm(output, p->code);
        p = p->next;
    }
}