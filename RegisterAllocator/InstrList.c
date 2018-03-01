#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "InstrList.h"
#include "Instr.h"

// size of line buffer
#define BUFSIZE 100

void PrintInstruction(Instruction *instr) {

}

void PrintInstructionList(Instruction *head) {
    Instruction *ptr = NULL;

    if (!head) {
        fprintf(stderr, "error: no instructions given");
        exit(EXIT_FAILURE);
    }
    ptr = head;
    while (ptr) {
        PrintInstruction(ptr);
        ptr = ptr->next;
    }
}

Instruction *ReadInstruction(FILE *infile) {
    Instruction *instr = NULL;
    static char buff[BUFSIZE];
    char dummy;
    char *str;

    if (!infile) {
        fprintf(stderr, "error: file error\n");
        exit(EXIT_FAILURE);
    }

    instr = calloc(1, sizeof(Instruction));
    if (!instr) {
        fprintf(stderr, "error: calloc allocation failed\n");
        exit(EXIT_FAILURE);
    }
    instr->next = NULL;
    instr->prev = NULL;
    // read line from infile
    fscanf(infile, " %99s", buff);
    printf("%s\n", buff);
    if (strnlen(buff, sizeof(buff)) == 0) {
        // line is empty
        free(instr);
        return NULL;
    }

    if (!strcmp(buff, "loadAI")) {
        // loadAI r1, c1 => r2
        instr->opcode = LOADAI;
        fscanf(infile, "%c%d , %d => %c%d", &dummy, &(instr->field1),
                                             &(instr->field2),
                                             &dummy, &(instr->field3));
    }
    else if (!strcmp(buff, "loadI")) {
        // loadI c1 => r1
        fscanf(infile, "%d => %c%d", &(instr->field1), 
                                     &dummy, &(instr->field2));
    }
    else if (!strcmp(buff, "load")) { 
        // load r1 => r2
        fscanf(infile, "%c%d => %c%d", &dummy, &(instr->field1), 
                                        &dummy, &(instr->field2));
    }
    else if (!strcmp(buff, "storeAI")) {
        // storeAI r1 => r2, c3
        instr->opcode = STOREAI;
        fscanf(infile, "%c%d => %c%d, %d", &dummy, &(instr->field1), 
                                            &dummy, &(instr->field2),
                                            &(instr->field3));
    }
    else if (!strcmp(buff, "store")) {
        // store r1 => r2
        instr->opcode = STORE;
        fscanf(infile, "%c%d => %c%d", &dummy, &(instr->field1), 
                                        &dummy, &(instr->field2));
    }
    else if (!strcmp(buff, "add")) {
        // add r1, r2 => r3
        instr->opcode = ADD;
        fscanf(infile, "%c%d , %c%d => %c%d",&dummy, &(instr->field1), 
                                             &dummy, &(instr->field2),
                                             &dummy, &(instr->field3));
    }
    else if (!strcmp(buff, "sub")) {
        // sub r1, r2 => r3
        instr->opcode = SUB;
        fscanf(infile, "%c%d , %c%d => %c%d", &dummy, &(instr->field1), 
                                              &dummy, &(instr->field2),
                                              &dummy, &(instr->field3));
    }
    else if (!strcmp(buff, "mul")) {
        // mul r1, r2 => r3
        instr->opcode = MUL;
        fscanf(infile, "%c%d , %c%d => %c%d", &dummy, &(instr->field1), 
                                              &dummy, &(instr->field2),
                                              &dummy, &(instr->field3));
    }
    else if (!strcmp(buff, "div")) {
        // sub r1, r2 => r3
        instr->opcode = DIV;
        fscanf(infile, "%c%d , %c%d => %c%d", &dummy, &(instr->field1), 
                                              &dummy, &(instr->field2),
                                              &dummy, &(instr->field3));
    }
    else if (!strcmp(buff, "lshift")) {
        // lshift r1, r2 => r3
        instr->opcode = LSHIFT;
        fscanf(infile, "%c%d , %c%d => %c%d", &dummy, &(instr->field1), 
                                              &dummy, &(instr->field2),
                                              &dummy, &(instr->field3));
    }
    else if (!strcmp(buff, "rshift")) {
        // rshift r1, r2 => r3
        instr->opcode = RSHIFT;
        fscanf(infile, "%c%d , %c%d => %c%d", &dummy, &(instr->field1), 
                                              &dummy, &(instr->field2),
                                              &dummy, &(instr->field3));
    }
    else if (!strcmp(buff, "output")) {
        // output r1, c1
        instr->opcode = OUTPUT;
        fscanf(infile, "%c%d , %c%d", &dummy, &(instr->field1), 
                                      &dummy, &(instr->field2));
    }
    else {
        free(instr);
        return NULL;
    }
    return instr;
}

Instruction *ReadInstructionList(FILE *infile) {
    Instruction *instr, *head, *tail;

    if (!infile) {
        fprintf(stderr, "error: file error\n");
        exit(EXIT_FAILURE);
    }
    head = tail = NULL;
    while ((instr = ReadInstruction(infile))) {
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
