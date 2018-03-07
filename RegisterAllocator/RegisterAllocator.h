#include "Instr.h"

#ifndef REGISTER_ALLOC_H
#define REGISTER_ALLOC_H

void usage();

void TopDownAlloc(Instruction *, int, int);
void BottomUpAlloc(Instruction *, int);
void SimpleAlloc(Instruction *, int *, int, int);
void MaxLiveAllocSpill(Instruction *, int *, int, int);
void MaxLiveAllocPhysical(Instruction *, int *, int, int);

void CountFrequencies(Instruction *, int *);
int CountRegisters(int *);
void PrioritySort(int *, int *, int);
int PriorityIndex(int *, int, int);


void LiveRange(Instruction *, int *);
void LiveRangeReg(Instruction *, int);

int MAXLIVE(Instruction *);

void InsertLoadAI(Instruction *, int, int, int );
void InsertStoreAI(Instruction *, int, int, int );

#endif
