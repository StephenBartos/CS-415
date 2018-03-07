#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "InstrList.h"
#include "Instr.h"

void PrintInstruction(Instruction *instr) {
    if (!instr) {
        fprintf(stderr, "error: instruction list empty\n");
        exit(EXIT_FAILURE);
    }
    switch (instr->opcode) {
        case LOAD:
            fprintf(stdout, "load r%d => r%d\n",        instr->field1, 
                                                        instr->field2);
            break;

        case LOADI:
            fprintf(stdout, "loadI %d => r%d\n",        instr->field1, 
                                                        instr->field2);
            break;

        case LOADAI:
            fprintf(stdout, "loadAI r%d, %d => r%d\n",  instr->field1, 
                                                        instr->field2,
                                                        instr->field3);
            break;

        case STORE:
            fprintf(stdout, "store r%d => r%d\n",       instr->field1, 
                                                        instr->field2);
            break;

        case STOREAI:
            fprintf(stdout, "storeAI r%d => r%d, %d\n", instr->field1, 
                                                        instr->field2,
                                                        instr->field3);
            break;

        case ADD:
            fprintf(stdout, "add r%d, r%d => r%d\n",    instr->field1, 
                                                        instr->field2,
                                                        instr->field3);
            break;

        case SUB:
            fprintf(stdout, "sub r%d, r%d => r%d\n",    instr->field1, 
                                                        instr->field2,
                                                        instr->field3);
            break;

        case MUL:
            fprintf(stdout, "mul r%d, r%d => r%d\n",    instr->field1, 
                                                        instr->field2,
                                                        instr->field3);
            break;

        case DIV:
            fprintf(stdout, "div %d, r%d => r%d\n",     instr->field1, 
                                                        instr->field2,
                                                        instr->field3);
            break;

        case LSHIFT:
            fprintf(stdout, "lshift %d, r%d => r%d\n",  instr->field1, 
                                                        instr->field2,
                                                        instr->field3);
            break;

        case RSHIFT:
            fprintf(stdout, "rshift %d, r%d => r%d\n",  instr->field1, 
                                                        instr->field2,
                                                        instr->field3);
            break;

        case OUTPUT:
            fprintf(stdout, "output %d\n",          instr->field1);
            break;

        default:
            fprintf(stderr, "error: illegal instruction\n");
            break;
    }
}

void PrintInstructionList(Instruction *head) {
    Instruction *ptr = NULL;

    if (!head) {
        fprintf(stderr, "error: no instructions given\n");
        exit(EXIT_FAILURE);
    }
    ptr = head;
    while (ptr) {
        PrintInstruction(ptr);
        ptr = ptr->next;
    }
}

Instruction *ReadInstruction(char *buff) {
    Instruction *instr = NULL;
    char dummy;
    char opcode[10];

    instr = calloc(1, sizeof(Instruction));
    if (!instr) {
        fprintf(stderr, "error: calloc allocation failed\n");
        exit(EXIT_FAILURE);
    }
    instr->next = NULL;
    instr->prev = NULL;
    memset(instr->live, 0, sizeof(instr->live));
    if (strnlen(buff, sizeof(buff)) == 0) {
        // line is empty
        free(instr);
        return NULL;
    }
    sscanf(buff, "%s", opcode);
    if (!strncmp(opcode, "loadAI", 6)) {
        // loadAI r1, c1 => r2
        instr->opcode = LOADAI;
        sscanf(buff, "%s %c%d , %d => %c%d",   opcode, 
                                               &dummy, &(instr->field1),
                                               &(instr->field2), &dummy, 
                                               &(instr->field3));
    }
    else if (!strncmp(opcode, "loadI", 5)) {
        // loadI c1 => r1
        instr->opcode = LOADI;
        sscanf(buff, "%s %d => %c%d",          opcode,
                                               &(instr->field1), 
                                               &dummy, &(instr->field2));
    }
    else if (!strncmp(opcode, "load", 4)) { 
        // load r1 => r2
        instr->opcode = LOAD;
        sscanf(buff, "%s %c%d => %c%d",        opcode, &dummy, &(instr->field1), 
                                               &dummy, &(instr->field2));
    }
    else if (!strncmp(opcode, "storeAI", 7)) {
        // storeAI r1 => r2, c3
        instr->opcode = STOREAI;
        sscanf(buff, "%s %c%d => %c%d, %d",    opcode, &dummy, &(instr->field1), 
                                               &dummy, &(instr->field2),
                                               &(instr->field3));
    }
    else if (!strncmp(opcode, "store", 5)) {
        // store r1 => r2
        instr->opcode = STORE;
        sscanf(buff, "%s %c%d => %c%d",        opcode, &dummy, &(instr->field1), 
                                               &dummy, &(instr->field2));
    }
    else if (!strncmp(opcode, "add", 3)) {
        // add r1, r2 => r3
        instr->opcode = ADD;
        sscanf(buff, "%s %c%d , %c%d => %c%d", opcode, &dummy, &(instr->field1), 
                                               &dummy, &(instr->field2),
                                               &dummy, &(instr->field3));
    }
    else if (!strncmp(opcode, "sub", 3)) {
        // sub r1, r2 => r3
        instr->opcode = SUB;
        sscanf(buff, "%s %c%d , %c%d => %c%d", opcode, &dummy, &(instr->field1), 
                                               &dummy, &(instr->field2), 
                                               &dummy, &(instr->field3));
    }
    else if (!strncmp(opcode, "mul", 3)) {
        // mul r1, r2 => r3
        instr->opcode = MUL;
        sscanf(buff, "%s %c%d , %c%d => %c%d", opcode, &dummy, &(instr->field1), 
                                               &dummy, &(instr->field2),
                                               &dummy, &(instr->field3));
    }
    else if (!strncmp(opcode, "div", 3)) {
        // sub r1, r2 => r3
        instr->opcode = DIV;
        sscanf(buff, "%s %c%d , %c%d => %c%d", opcode, &dummy, &(instr->field1), 
                                               &dummy, &(instr->field2),
                                               &dummy, &(instr->field3));
    }
    else if (!strncmp(opcode, "lshift", 6)) {
        // lshift r1, r2 => r3
        instr->opcode = LSHIFT;
        sscanf(buff, "%s %c%d , %c%d => %c%d", opcode, &dummy, &(instr->field1), 
                                               &dummy, &(instr->field2),
                                               &dummy, &(instr->field3));
    }
    else if (!strncmp(opcode, "rshift", 6)) {
        // rshift r1, r2 => r3
        instr->opcode = RSHIFT;
        sscanf(buff, "%s %c%d , %c%d => %c%d", opcode, &dummy, &(instr->field1), 
                                               &dummy, &(instr->field2),
                                               &dummy, &(instr->field3));
    }
    else if (!strncmp(opcode, "output", 6)) {
        // output r1, c1
        instr->opcode = OUTPUT;
        sscanf(buff, "%s %d",                  opcode, &(instr->field1));
    }
    else {
        free(instr);
        return NULL;
    }
    return instr;
}

Instruction *ReadInstructionList(FILE *infile) {
    Instruction *instr, *head, *tail;
    char buff[100];

    if (!infile) {
        fprintf(stderr, "error: file error\n");
        exit(EXIT_FAILURE);
    }

    head = tail = NULL;
    while (fscanf(infile, " %99[^\n]", buff) != EOF) {
        instr = ReadInstruction(buff);
        if (!instr) {
            continue;
        }
        if (!head) {
            head = tail = instr;
            continue;
        }
        instr->prev = tail;
        instr->next = NULL;
        tail->next = instr;
        tail = instr;
    }
    return head;
}

Instruction *LastInstruction(Instruction *instr) {
    if (!instr) {
        fprintf(stderr, "error: no instructions given\n");
        exit(EXIT_FAILURE);
    }
    while (instr->next) {
        instr = instr->next;
    }
    return instr;
}

void DestroyInstructionList(Instruction *instr) {
    Instruction *ptr;
    if (!instr) {
        return;
    }
    while (instr) {
        ptr = instr;
        instr = instr->next;
        free(ptr);
    }
}
