#include <stdio.h>
#include "interCodes.h"
#include "help.h"
#include "objectCode.h"

extern InterCodesList list;
static ObjectCodePtr ocHead;

#define REG_NUM 32
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
#define REG(X) regs[X]

void printStartCode(FILE *output)
{
    FILE *startcode = fopen("startCode.s", "r");
    int c;
    while ((c = fgetc(startcode)) != EOF)
    {
        fputc(c, output);
    }
    fputc('\n', output);
}

/*
move $v0, reg(x)
jr $ra
if (p->u.oneop.op->kind == OP_ADDRESS)
    fprintf(output, "*");
printOp(output, p->u.oneop.op);
*/

/*
x := #k  li reg(x), k
x := y   move reg(x), reg(y)
*/

/*
x := y + #k     addi reg(x), reg(y), k
x := y + z      add reg(x), reg(y), reg(z)
x := y - #k     addi reg(x), reg(y), -k
x := y – z      sub reg(x), reg(y), reg(z)
x := y * z2     mul reg(x), reg(y), reg(z)
x := y / z      div reg(y), reg(z)
                mflo reg(x)
*/

/*
IF x == y GOTO z    beq reg(x), reg(y), z
IF x != y GOTO z    bne reg(x), reg(y), z
IF x > y GOTO z     bgt reg(x), reg(y), z
IF x < y GOTO z     blt reg(x), reg(y), z
IF x >= y GOTO z    bge reg(x), reg(y), z
IF x <= y GOTO z    ble reg(x), reg(y), z
*/

void genObjectCode(FILE *output, InterCodePtr p)
{
    switch (p->kind)
    {
    case IR_FUNCTION:
        ObjectCodePtr ptr = newObjectCode(OC_FUNC, mystrdup(p->u.oneop.op->u.name));
        addObjectCode(ocHead, ptr);
        break;
    case IR_LABEL:
        ObjectCodePtr ptr = newObjectCode(OC_LABEL, mystrdup(p->u.oneop.op->u.name));
        addObjectCode(ocHead, ptr);
        break;

    case IR_PARAM:
        fprintf(output, "PARAM ");
        printOp(output, getOneOpOp(p));
        break;

    case IR_DEC:
        fprintf(output, "DEC %s %d", p->u.dec.varName->u.name, p->u.dec.size);
        break;

    case IR_ASSIGN:
        int regx; // TODO:
        if (p->u.assign.right->kind == OP_CONSTANT)
        {
            ObjectCodePtr ptr = newObjectCode(OC_LI, regx, p->u.assign.right->u.value);
            addObjectCode(ocHead, ptr);
        }
        else
        {
            int regy; // TODO:
            ObjectCodePtr ptr = newObjectCode(OC_MOVE, regx, regy);
            addObjectCode(ocHead, ptr);
        }
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
        
        break;
    case IR_SUB:
    case IR_MUL:
    case IR_DIV:

        break;

    case IR_RETURN:
        break;

    case IR_GOTO:
        fprintf(output, "j ");
        printOp(output, getOneOpOp(p));
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

#define STR(x) #x
#define OP_RRR(instr, r1, r2, r3) fprintf(output, STR(instr) " %s, %s, %s", REG(r1), REG(r2), REG(r3));
#define OP_RRI(instr, r1, r2, imm) fprintf(output, STR(instr) " %s, %s, %d", REG(r1), REG(r2), imm);
#define OP_RR(instr, r1, r2) fprintf(output, STR(instr) " %s, %s", REG(r1), REG(r2));
#define OP_R(instr, r1) fprintf(output, STR(instr) " %s", REG(r1));
#define OP_ROR(instr, r1, offset, r2) fprintf(output, STR(instr) " %s, %d(%s)", REG(r1), offset, REG(r2));

void printObjectCode(FILE *output, ObjectCodePtr p)
{
    switch (p->kind)
    {
    case OC_FUNC:
        fprintf(output, "%s:", p->func.funcname);
        break;
    case OC_LABEL:
        fprintf(output, "%s", p->label.x);
        break;
    case OC_ADD: // add reg(x), reg(y), reg(z)
        OP_RRR(add, p->add.regx, p->add.regy, p->add.regz)
        break;
    case OC_ADDI: // addi reg(x), reg(y), k
        OP_RRI(addi, p->addi.regx, p->addi.regy, p->addi.imm)
        break;
    case OC_SUB: // sub reg(x), reg(y), reg(z)
        OP_RRR(sub, p->add.regx, p->add.regy, p->add.regz)
        break;
    case OC_MUL: // mul reg(x), reg(y), reg(z)
        OP_RRR(mul, p->mul.regx, p->mul.regy, p->mul.regz);
        break;
    case OC_DIV: // div reg(y), reg(z)
        OP_RR(div, p->div.regy, p->div.regz)
        break;
    case OC_MFLO: // mflo reg(x)
        OP_R(mflo, p->mflo.regx)
        break;
    case OC_LW: // lw reg(x), 0(reg(y))
        OP_ROR(lw, p->lw.regx, p->lw.offset, p->lw.regy)
        break;
    case OC_SW: // sw reg(y), 0(reg(x))
        OP_ROR(sw, p->sw.regy, p->sw.offset, p->sw.regx)
        break;
    case OC_J:
        fprintf(output, "j label%d", p->j.x);
        break;
    case OC_JAL:
        fprintf(output, "jal %s", p->jal.f);
        break;
    case OC_JR:
        fprintf(output, "jr $ra");
        break;
    case OC_BEQ: // beq reg(x), reg(y), z
        OP_RRI(beq, p->beq.regx, p->beq.regy, p->beq.z)
        break;
    case OC_BNE:
        OP_RRI(bne, p->bne.regx, p->bne.regy, p->bne.z)
        break;
    case OC_BGT:
        OP_RRI(bgt, p->bgt.regx, p->bgt.regy, p->bgt.z)
        break;
    case OC_BLT:
        OP_RRI(blt, p->blt.regx, p->blt.regy, p->blt.z)
        break;
    case OC_BGE:
        OP_RRI(bge, p->bge.regx, p->bge.regy, p->bge.z)
        break;
    case OC_BLE:
        OP_RRI(ble, p->ble.regx, p->ble.regy, p->ble.z)
        break;
    }
    fprintf(output, "\n");
}

void printObjectCodes(FILE *output)
{
    printStartCode(output);
    ObjectCodePtr p = ocHead->next;
    while (p != ocHead)
    {
        printObjectCode(output, p);
        p = p->next;
    }
}