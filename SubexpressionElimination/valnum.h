/**********************************************
        CS415  Project 3
        Spring  2018
        Student Version
**********************************************/

#ifndef VALNUM_H
#define VALNUM_H

#include <string.h>
#include "instrutil.h"

/* INSERT WHATEVER YOU NEED FOR THE VALUE NUMBER HASH FUNCTION */

typedef struct _HashTableEntry {
    char *name;
    int offset;
} HashTableEntry;

extern void InitHashTable();

HashTableEntry *HashTableLookup(char *name);

extern void HashTableInsert(char *name, int offset);

extern void PrintHashTable();

#endif
