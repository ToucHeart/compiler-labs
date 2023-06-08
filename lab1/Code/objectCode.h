#ifndef __OBJECT_CODE__
#define __OBJECT_CODE__
#include<stdbool.h>

typedef enum
{
    OC_NONE,
    OC_FUNC,
    OC_LABEL,
    OC_LI,
    OC_MOVE,
    OC_ADD,
    OC_ADDI,
    OC_SUB,
    OC_MUL,
    OC_DIV,
    OC_MFLO,
    OC_LW,
    OC_SW,
    OC_J,
    OC_JAL,
    OC_JR,
    OC_BEQ,
    OC_BNE,
    OC_BGT,
    OC_BLT,
    OC_BGE,
    OC_BLE,
    OC_LA,
} OC_Kind;

typedef struct ObjectCode
{
    OC_Kind kind;
    union
    {
        struct
        {
            char* x; // lebel index
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
ObjectCodePtr newObjectCode(OC_Kind k, ...);


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
    int reg;
    int offset;
    struct Var* next;
} Var;
typedef Var* VarPtr;

typedef struct Register
{
    VarPtr var;
} Register;

#endif