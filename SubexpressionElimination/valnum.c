/**********************************************
        CS415  Project 3
        Spring  2018
        Student Version
**********************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "instrutil.h"
#include "valnum.h"

#define HASH_TABLE_SIZE 467

/*  --- Static VARIABLES AND FUNCTIONS --- */
static HashTableEntry **HashTable;

static int hash(char *name) {
    int i;
    int hashValue = 1;

    for (i=0; i < strlen(name); i++) {
        hashValue = (hashValue * name[i]) % HASH_TABLE_SIZE;
    }

    return hashValue;
}


void InitHashTable() {
    int i;
    //int dummy;

    HashTable = (HashTableEntry**) malloc (sizeof(HashTableEntry*) * HASH_TABLE_SIZE);
    for (i=0; i < HASH_TABLE_SIZE; i++)
        HashTable[i] = NULL;
}


/* Returns pointer to symbol table entry, if entry is found */
/* Otherwise, NULL is returned */
HashTableEntry *HashTableLookup(char *name) {
    int currentIndex;
    int visitedSlots = 0;

    currentIndex = hash(name);
    while (HashTable[currentIndex] != NULL && visitedSlots < HASH_TABLE_SIZE) {
        if (!strcmp(HashTable[currentIndex]->name, name) )
            return HashTable[currentIndex];
        currentIndex = (currentIndex + 1) % HASH_TABLE_SIZE; 
        visitedSlots++;
    }
    return NULL;
}


void HashTableInsert(char *name, int offset) {
    int currentIndex;
    int visitedSlots = 0;

    currentIndex = hash(name);
    while (HashTable[currentIndex] != NULL && visitedSlots < HASH_TABLE_SIZE) {
        if (!strcmp(HashTable[currentIndex]->name, name) ) 
            printf("*** WARNING *** in function \"insert\": %s has already an entry\n", name);
        currentIndex = (currentIndex + 1) % HASH_TABLE_SIZE; 
        visitedSlots++;
    }
    if (visitedSlots == HASH_TABLE_SIZE) {
        printf("*** ERROR *** in function \"insert\": No more space for entry %s\n", name);
        return;
    }

    HashTable[currentIndex] = (HashTableEntry*) malloc (sizeof(HashTableEntry));
    HashTable[currentIndex]->name = (char *) malloc (strlen(name)+1);
    strcpy(HashTable[currentIndex]->name, name);
    HashTable[currentIndex]->offset = offset; /* in bytes */
}
