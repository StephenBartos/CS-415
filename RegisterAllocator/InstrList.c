#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Instr.h"

void PrintInstruction(Instruction *instr) {

}

void PrintInstructionList(Instruction *head) {
    Instruction *ptr = NULL;
    if (!head) {
        fprintf(stderr, "No instructions given");
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
    return 0;
}
