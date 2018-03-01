#ifndef INSTR_H
#define INSTR_H

typedef enum {LOAD, LOADI, LOADAI, STORE, STOREAI, ADD, SUB, MUL, DIV, LSHIFT, RSHIFT, OUTPUT} Opcode;

typedef struct _Instruction {
    Opcode opcode;
    int field1;
    int field2;
    int field3;
    struct _Instruction *prev;
    struct _Instruction *next;
} Instruction;

#endif

