#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "RegisterAllocator.h"
#include "InstrList.h"

int main(int argc, char **argv)
{
    Instruction *instr;
    FILE *fp;
    int k;

    if (argc != 4) {
        fprintf(stderr, "error: incorrect number of arguments\n");
        usage();
        exit(EXIT_FAILURE);
    }
    // open infile for reading
    fp = fopen(argv[3], "r");
    if (!fp) {
        fprintf(stderr, "error: file could not be opened/found\n");
        exit(EXIT_FAILURE);
    }
    // read infile for instruction list
    instr = ReadInstructionList(fp);
    if (!instr) {
        fprintf(stderr, "error: no instructions given\n");
        usage();
        exit(EXIT_FAILURE);
    }
PrintInstructionList(instr);
printf("---------------------------------------\n");
    k = atoi(argv[1]);
    if (strncmp(argv[2], "b", 1) == 0) {
        TopDownAllocSimple(instr, k);
    }
    else if (strncmp(argv[2], "s", 1) == 0) {
        TopDownAllocMAXLIVE(instr, k);
    }
    else if (strncmp(argv[2], "t", 1) == 0) {

    }
    else {
        fprintf(stderr, "error: allocator type flag invalid\n");
        usage();
        exit(EXIT_FAILURE);
    }
    PrintInstructionList(instr);
    // free instr list
    DestroyInstructionList(instr);
    // close infile
    fclose(fp);
    return 0;
}

void usage() {
    printf("usage: ./alloc num_registers b|s|t input_file\n");
}

// Given an pointer to the head of an instruction list and a pointer to an empty array of size 256,
// counts the frequency of each virtual register. vregs[i] corresponds to register r_[i+1]
void CountFrequencies(Instruction *head, int *vregs) {
    Instruction *instr = NULL;

    if (!head) {
        fprintf(stderr, "error: no instructions given\n");
        exit(EXIT_FAILURE);
    }
    // count the frequencies of each register
    instr = head;
    while (instr) {
        switch (instr->opcode) {
            case LOAD: case STORE:
                vregs[instr->field1-1]++;
                vregs[instr->field2-1]++;
                break;

            case LOADI:
                if (instr->field1 == 1024) {
                    break;
                }
                vregs[instr->field2-1]++;
                break;

            case LOADAI:
                vregs[instr->field1-1]++;
                vregs[instr->field3-1]++;
                break;

            case STOREAI:
                vregs[instr->field1-1]++;
                vregs[instr->field2-1]++;
                break;

            case ADD: case SUB: 
            case MUL: case DIV:
            case LSHIFT: 
            case RSHIFT:
                vregs[instr->field1-1]++;
                vregs[instr->field2-1]++;
                vregs[instr->field3-1]++;
                break;

            default:
                break;
        }
        instr = instr->next;
    }

}

// Given an array containting the frequencies of each virtual register, returns the total number of
// virtual registers used.
int CountRegisters(int *vregs) {
    int result, n;
    result = n = 0;
    if (!vregs) {
        return 0;
    }
    for (int i = 0; i < 255; i++) {
        if (vregs[i]) {
            result++;
        }
    }
    return result;
}

// Sorts the virtual registers into priority order -- i.e., highest frequency will be 
void PrioritySort(int *vregs, int *pri_regs, int num_regs) {
    int max = -1, index = -1;
    for (int i = 0; i < num_regs; i ++) {
        for (int j = 0; j < 255; j++) {
            if (!vregs[j]) {
                continue;
            }
            if (vregs[j] > max) {
                max = vregs[j];
                index = j;
            }
        }
        pri_regs[i] = index + 1;
        vregs[index] = 0;
        max = -1;
    }
}

void SimpleAlloc(Instruction *head, int *pri_regs, int num_regs, int k) {
    Instruction *instr = NULL;
    if (!head) {
        fprintf(stderr, "error: no instructions given\n");
        exit(EXIT_FAILURE);
    }
    instr = head;
    while (instr) {
        int reg = 0;
        switch (instr->opcode) {
            case LOAD: case STORE:
                reg = PriorityIndex(pri_regs, num_regs, instr->field1) + 1;
                instr->field1 = reg;
                if (reg > (k - 2)) {
                    InsertLoadAI(instr, reg, num_regs);
                }
                reg = PriorityIndex(pri_regs, num_regs, instr->field2) + 1;
                instr->field2 = reg;
                if (reg > (k - 2)) {
                    InsertStoreAI(instr, reg, num_regs);
                    instr = instr->next;
                }
                break;

            case LOADI:
                if (instr->field1 == 1024) {
                    break;
                }
                reg = PriorityIndex(pri_regs, num_regs, instr->field2) + 1;
                instr->field2 = reg;
                if (reg > (k - 2)) {
                    InsertStoreAI(instr, reg, num_regs);
                    instr = instr->next;
                }
                break;

            case LOADAI:
                reg = PriorityIndex(pri_regs, num_regs, instr->field3) + 1;
                instr->field3 = reg;
                break;

            case STOREAI:
                reg = PriorityIndex(pri_regs, num_regs, instr->field1) + 1;
                instr->field1 = reg;
                break;


            case ADD: case SUB: 
            case MUL: case DIV:
            case LSHIFT: 
            case RSHIFT:
                reg = PriorityIndex(pri_regs, num_regs, instr->field1) + 1;
                instr->field1 = reg;
                if (reg > (k - 2)) {
                    InsertLoadAI(instr, reg, num_regs);
                }

                reg = PriorityIndex(pri_regs, num_regs, instr->field2) + 1;
                instr->field2 = reg;
                if (reg > (k - 2)) {
                    InsertLoadAI(instr, reg, num_regs);
                }

                reg = PriorityIndex(pri_regs, num_regs, instr->field3) + 1;
                instr->field3 = reg;
                if (reg > (k - 2)) {
                    InsertStoreAI(instr, reg, num_regs);
                    instr = instr->next;
                }
                break;

            default:
                break;
        }
        instr = instr->next;
    }
}
int PriorityIndex(int *pri_regs, int num_regs, int reg) {
    for(int i = 0; i < num_regs; i++) {
        if (pri_regs[i] == reg) {
            return i;
        }
    }
    return -1;
}

void InsertLoadAI(Instruction *instr, int reg, int num_regs) {
    Instruction *load = NULL;
    load = calloc(1, sizeof(Instruction));
    if (!load) { 
        fprintf(stderr, "error: calloc allocation failed\n");
    }
    printf("reg: %d\n", reg);
    load->opcode = LOADAI;
    load->field1 = 0;
    load->field2 = (-4) * (num_regs - reg);
    load->field3 = reg;
    load->prev = instr->prev;
    load->next = instr;

    (instr->prev)->next = load;
    instr->prev = load;
}

void InsertStoreAI(Instruction *instr, int reg, int num_regs) {
    Instruction *store = NULL;
    store = calloc(1, sizeof(Instruction));
    if (!store) { 
        fprintf(stderr, "error: calloc allocation failed\n");
    }
    store->opcode = STOREAI;
    store->field1 = reg;
    store->field2 = 0;
    store->field3 = (-4) * (num_regs - reg);
    store->prev= instr;
    store->next= instr->next;

    (instr->next)->prev= store;
    instr->next = store;
}

void TopDownAllocSimple(Instruction *head, int k) {
    // frequency counts for virtual registers
    int vregs[256] = {0};
    int num_regs = 0;

    CountFrequencies(head, vregs);
    num_regs = CountRegisters(vregs);

    int pri_regs[num_regs];
    PrioritySort(vregs, pri_regs, num_regs);
    for (int i = 0; i < num_regs; i++) {
        printf("%d, ", pri_regs[i]);
    }
    printf("\n\n");

    SimpleAlloc(head, pri_regs, num_regs, k);
}

void TopDownAllocMAXLIVE(Instruction * head, int k) {

}

void BottomUpAlloc(Instruction *head, int k) {

}
