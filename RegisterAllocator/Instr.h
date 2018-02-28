#ifndef INSTR_H
#define INSTR_H

typedef enum {load, loadI, loadAI, store, storeAI, add, sub, mul, div, lshift, rshift, output} OpCode;


typedef struct _Instruction {
    OpCode opcode;
    int field1;
    int field2;
    int field3;

    struct _Instruction *prev;
    struct _Instruction *next;
} Instruction;

#endif
