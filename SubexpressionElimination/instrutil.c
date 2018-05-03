/**********************************************
  CS415  Project 3
  Spring  2018
Author: Ulrich Kremer
 **********************************************/

#include <stdio.h>
#include <stdlib.h>
#include "instrutil.h"
#include "valnum.h"

static int next_register = 1; /* register 0 is reserved */
static int next_label = 0;
static int next_offset = 0;
static int is_init = 0;

int NextRegister() 
{
    if (next_register < MAX_VIRTUAL_REGISTERS)
        return next_register++;
    else {
        printf("*** ERROR *** Reached limit of virtual registers: %d\n", next_register);
        exit(-1);
    }
}

int NextLabel() 
{
    return next_label++;
}

int NextOffset(int units) 
{ 
    int current_offset = next_offset;
    next_offset = next_offset + 4*units;
    return current_offset;
}

void emitComment(char *comment)
{
    fprintf(outfile, "\t// %s\n", comment);  
}


/*
 * emit implements CSE
 */

int emit(int label_index,
        Opcode_Name opcode, 
        int field1, 
        int field2, 
        int field3) 
{
    char *label = " ";
    char buf[256] = "";
    char buf2[256] = "";
    HashTableEntry *entry = NULL;

    if (label_index < NOLABEL) {
        printf("ERROR: \"%d\" is an illegal label index.\n", label_index);
        return -1;
    }

    if (label_index > NOLABEL) {
        label = (char *) malloc(100);
        sprintf(label, "L%d:", label_index);
    };


    if (!cse_optimization_flag) {
        switch (opcode) { /* ---------------------- NON OPTIMIZED ------------------------------- */
            case NOP: 
                fprintf(outfile, "%s\t nop \n", label);
                return -1;
                break;
            case ADD:
                fprintf(outfile, "%s\t add r%d, r%d \t=> r%d \n", label, field1, field2, field3);
                return field3;
                break;
            case SUB: 
                fprintf(outfile, "%s\t sub r%d, r%d \t=> r%d \n", label, field1, field2, field3);
                return field3;
                break;
            case MULT: 
                fprintf(outfile, "%s\t mult r%d, r%d \t=> r%d \n", label, field1, field2, field3);
                return field3;
                break;
            case LOADI: 
                /* Example: loadI 1024 => r1 */
                fprintf(outfile, "%s\t loadI %d \t=> r%d \n", label, field1, field2);
                return field2;
                break;
            case LOADAI: 
                /* Example: loadAI r1, 16 => r3 */
                fprintf(outfile, "%s\t loadAI r%d, %d \t=> r%d \n", label, field1, field2, field3);
                return field3;
                break;
            case STOREAI: 
                /* Example: storeAI r1 => r2, 16 */
                fprintf(outfile, "%s\t storeAI r%d \t=> r%d, %d \n", label, field1, field2, field3);
                break;
            case OUTPUTAI: 
                /* Example: outputAI r0, 16  */
                fprintf(outfile, "%s\t outputAI r%d, %d\n", label, field1, field2);
                break;
            default:
                fprintf(stderr, "Illegal instruction in \"emit\" \n");
        }
        return -1;
    }

    else {
        if (!is_init) {
            InitHashTable();
            is_init= 1;
        }
        switch (opcode) { /* ---------------------- CSE OPTIMIZED ------------------------------- */
            case NOP: 
                fprintf(outfile, "%s\t nop \n", label);
                return -1;
                break;
            case ADD:
                sprintf(buf, "add_%d_%d", field1, field2);
                sprintf(buf2, "add_%d_%d", field2, field1);
                entry = HashTableLookup(buf);
                if (!entry) {
                    // entry is not found, insert both variants into HashTable and emit instruction
                    HashTableInsert(buf, field3);
                    HashTableInsert(buf2, field3);
                    fprintf(outfile, "%s\t add r%d, r%d \t=> r%d \n", label, field1, field2, field3);
                    return field3;
                }
                // return entry's target register
                return entry->offset;
                break;
            case SUB: 
                sprintf(buf, "sub%d_%d", field1, field2);
                entry = HashTableLookup(buf);
                if (!entry) {
                    // entry is not found, insert both variants into HashTable and emit instruction
                    HashTableInsert(buf, field3);
                    fprintf(outfile, "%s\t sub r%d, r%d \t=> r%d \n", label, field1, field2, field3);
                    return field3;
                }
                // return entry's target register
                return entry->offset;
                break;
            case MULT: 
                sprintf(buf, "mult%d_%d", field1, field2);
                sprintf(buf2, "mult%d_%d", field2, field1);
                entry = HashTableLookup(buf);
                if (!entry) {
                    // entry is not found, insert both variants into HashTable and emit instruction
                    HashTableInsert(buf, field3);
                    HashTableInsert(buf2, field3);
                    fprintf(outfile, "%s\t mult r%d, r%d \t=> r%d \n", label, field1, field2, field3);
                    return field3;
                }
                // return entry's target register
                return entry->offset;
                break;
            case LOADI: 
                /* Example: loadI 1024 => r1 */
                sprintf(buf, "loadI_%d", field1);
                entry = HashTableLookup(buf);
                if (!entry) {
                    // entry is not found, insert it into HashTable and return the target register
                    HashTableInsert(buf, field2);
                    fprintf(outfile, "%s\t loadI %d \t=> r%d \n", label, field1, field2);
                    return field2;
                }
                // return entry's target register
                return entry->offset;
                break;
            case LOADAI: 
                /* Example: loadAI r1, 16 => r3 */
                sprintf(buf, "storeAI_r%d,%d", field1, field2);
                entry = HashTableLookup(buf);
                if (!entry) {
                    // should NOT happen, as every loadAI requires a previous storeAI
                    // entry is not found, emit instruction and return target the register
                    fprintf(outfile, "%s\t loadAI r%d, %d \t=> r%d \n", label, field1, field2, field3);
                    return field3;
                }
                // return entry's target register
                return entry->offset;
                break;
            case STOREAI: 
                /* Example: storeAI r1 => r2, 16 */
                sprintf(buf, "storeAI_r%d,%d", field2, field3);
                entry = HashTableLookup(buf);
                if (!entry) {
                    // entry is not found, insert it into HashTable
                    HashTableInsert(buf, field1);
                }
                else {
                    // update target register of entry
                    entry->offset = field1;
                }
                // emit instruction and return the target register
                fprintf(outfile, "%s\t storeAI r%d \t=> r%d, %d \n", label, field1, field2, field3);
                return field1;
                break;
            case OUTPUTAI: 
                /* Example: outputAI r0, 16  */
                fprintf(outfile, "%s\t outputAI r%d, %d\n", label, field1, field2);
                break;
            default:
                fprintf(stderr, "Illegal instruction in \"emit\" \n");
        }
        return -1;
    }
}




