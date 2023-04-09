#define HASH_TABLE_SIZE 0x3fff

typedef enum
{
    VAR,
    BASIC,
    ARRAY,
    STRUCTURE,
    FUNCTION,
}Kind;

typedef struct Type
{
    Kind kind;
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
        struct FieldList* structure;
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

// 结构体类型信息
typedef struct FieldList
{
    char* name;             // 域的名字
    Type* type;             // 域的类型
    struct FieldList* tail; // 下一个域
} FieldList;