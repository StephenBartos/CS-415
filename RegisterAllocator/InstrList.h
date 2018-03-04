#include "Instr.h"
#include <stdio.h>

#ifndef INSTR_LIST_H
#define INSTR_LIST_H

void PrintInstruction(Instruction *);
void PrintInstructionList(Instruction *);
Instruction *ReadInstruction(char *);
Instruction *ReadInstructionList(FILE *);

#endif
