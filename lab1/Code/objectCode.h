#ifndef __OBJECT_CODE__
#define __OBJECT_CODE__
#include<stdbool.h>

typedef enum
{
    ASM_NONE,
    ASM_FUNC,
    ASM_LABEL,
    ASM_LI,
    ASM_MOVE,
    ASM_ADD,
    ASM_ADDI,
    ASM_SUB,
    ASM_MUL,
    ASM_DIV,
    ASM_MFLO,
    ASM_LW,
    ASM_SW,
    ASM_J,
    ASM_JAL,
    ASM_JR,
    ASM_BEQ,
    ASM_BNE,
    ASM_BGT,
    ASM_BLT,
    ASM_BGE,
    ASM_BLE,
    ASM_LA,
} ASM_Kind;

typedef struct ObjectCode
{
    ASM_Kind kind;
    union
    {
        struct
        {
            int x; // lebel index
        } label;
        struct
        {
            int reg, imm;
        } li;
        struct
        {
            int regx, regy;
        } move;
        struct
        {
            int regx, regy, imm;
        } addi;
        struct
        {
            int regx, regy, regz;
        } add;
        struct
        {
            int regx, regy, regz;
        } sub;
        struct
        {
            int regx, regy, regz;
        } mul;
        struct
        {
            int regy, regz;
        } div;
        struct
        {
            int regx;
        } mflo;
        struct
        {
            int regx, offset, regy;
        } lw;
        struct
        {
            int regy, offset, regx;
        } sw;
        struct
        {
            int label;
        } j;
        struct
        {
            char* f;
        } jal;
        struct
        {
            int regx, regy, z;
        } beq;
        struct
        {
            int regx, regy, z;
        } bne;
        struct
        {
            int regx, regy, z;
        } bgt;
        struct
        {
            int regx, regy, z;
        } blt;
        struct
        {
            int regx, regy, z;
        } bge;
        struct
        {
            int regx, regy, z;
        } ble;
        struct
        {
            char* funcname;
        } func;
        struct
        {
            int reg;
            char* name;
        }la;
    };
    struct ObjectCode* prev, * next;
} ObjectCode;
typedef ObjectCode* ObjectCodePtr;
void addObjectCode(ObjectCodePtr dummyHead, ObjectCodePtr p);
ObjectCodePtr newObjectCode(ASM_Kind k, ...);


typedef enum regNo
{
    REG_ZERO,
    REG_AT,
    REG_V0,
    REG_V1,
    REG_A0,
    REG_A1,
    REG_A2,
    REG_A3,
    REG_T0,
    REG_T1,
    REG_T2,
    REG_T3,
    REG_T4,
    REG_T5,
    REG_T6,
    REG_T7,
    REG_S0,
    REG_S1,
    REG_S2,
    REG_S3,
    REG_S4,
    REG_S5,
    REG_S6,
    REG_S7,
    REG_T8,
    REG_T9,
    REG_K0,
    REG_K1,
    REG_GP,
    REG_SP,
    REG_FP,
    REG_RA,
    REG_NUM,
}regNo;

#define MIN_AVAIL_REG REG_T0
#define MAX_AVAIL_REG REG_K0


typedef struct Var
{
    char* name;
    int reg;//寄存器位置
    int offset;//内存位置
    struct Var* prev, * next;
} Var;
typedef Var* VarPtr;

typedef struct Register
{
    VarPtr var;
    bool isUsed;
    int type;
} Register;

#endif