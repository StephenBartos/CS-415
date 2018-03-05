#include "Instr.h"

#ifndef REGISTER_ALLOC_H
#define REGISTER_ALLOC_H

void usage();
void TopDownAllocSimple(Instruction *, int);
void TopDownAllocMAXLIVE(Instruction *, int);
void BottomUpAlloc(Instruction *, int);

void CountFrequencies(Instruction *, int *);
int CountRegisters(int *);
void PrioritySort(int *, int *, int);
int PriorityIndex(int *, int, int);
void InsertLoadAI(Instruction *, int, int );
void InsertStoreAI(Instruction *, int, int );
#endif
