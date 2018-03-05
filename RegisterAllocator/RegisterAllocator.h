#include "Instr.h"

#ifndef REGISTER_ALLOC_H
#define REGISTER_ALLOC_H

void usage();
void TopDownAllocSimple(Instruction *, int);
void TopDownAlloc(Instruction *, int);
void BottomUpAlloc(Instruction *, int);
void CountFrequencies(Instruction *, int *);
int CountRegisters(int *);

#endif
