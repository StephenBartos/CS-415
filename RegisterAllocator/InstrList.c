#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Instr.h"

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
    return 0;
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
