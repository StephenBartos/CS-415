#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "RegisterAllocator.h"
#include "InstrList.h"

#define NUM_VR_REGS 256

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
    if (strncmp(argv[2], "s", 1) == 0) {
        TopDownAlloc(instr, k, 0);
    }
    else if (strncmp(argv[2], "t", 1) == 0) {
        TopDownAlloc(instr, k, 1);
    }
    else if (strncmp(argv[2], "b", 1) == 0) {

    }
    else {
        fprintf(stderr, "error: allocator type flag invalid\n");
        usage();
        exit(EXIT_FAILURE);
    }
    // print allocated register list
    PrintInstructionList(instr);
    // free instr list
    DestroyInstructionList(instr);
    // close infile
    fclose(fp);
    return 0;
}

void usage() {
    fprintf(stdout, "usage: ./alloc num_registers b|s|t input_file\n");
}

// Top-Down allocator that allocates physical registers to virtual registers based on frequency,
// and the MAXLIVE heuristic IFF flag = 1
void TopDownAlloc(Instruction *head, int k, int flag) {
    // frequency counts for virtual registers
    int vregs_freq[NUM_VR_REGS] = {0};
    int num_regs = 0;

    CountFrequencies(head, vregs_freq);
    if (flag) {
        LiveRange(head, vregs_freq);
        num_regs = CountRegisters(vregs_freq);
        int pri_regs[num_regs];
        PrioritySort(vregs_freq, pri_regs, num_regs);
    for (int i = 0; i < num_regs; i++) {
        printf("r%d > ", pri_regs[i]);
    }
    printf("\n");

        MaxLiveAllocPhysical(head, pri_regs, num_regs, k);
    }
    else {
        num_regs = CountRegisters(vregs_freq);
        int pri_regs[num_regs];

        PrioritySort(vregs_freq, pri_regs, num_regs);
    /*
    for (int i = 0; i < num_regs; i++) {
        printf("r%d > ", pri_regs[i]);
    }
    printf("\n");
    */
        SimpleAlloc(head, pri_regs, num_regs, k);
    }
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
    for (int i = 0; i < NUM_VR_REGS; i++) {
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
        for (int j = 0; j < NUM_VR_REGS; j++) {
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
        int offset = 0;
        switch (instr->opcode) {
            case LOAD: case STORE:
                reg = PriorityIndex(pri_regs, num_regs, instr->field1) + 1;
                if (reg > (k - 2)) {
                    // reg must be spilled
                    offset = reg;
                    reg = k - 1;
                    instr->field1 = reg;
                    InsertLoadAI(instr, offset, reg, k);
                }
                else {
                    // assign reg a physical register
                    instr->field1 = reg;
                }

                reg = PriorityIndex(pri_regs, num_regs, instr->field2) + 1;
                if (reg > (k - 2)) {
                    // reg must be spilled
                    offset = reg;
                    reg = k;
                    instr->field2 = reg;
                    InsertStoreAI(instr, offset, reg, k);
                    instr = instr->next;
                }
                else {
                    // assign reg a physical register
                    instr->field2 = reg;
                }
                break;

            case LOADI:
                if (!instr->field2) {
                    break;
                }
                reg = PriorityIndex(pri_regs, num_regs, instr->field2) + 1;
                if (reg > (k - 2)) {
                    // reg must be spilled
                    offset = reg;
                    reg = k - 1;
                    instr->field2 = reg;
                    InsertStoreAI(instr, offset, reg, k);
                    instr = instr->next;
                }
                else {
                    // assign reg a physical register
                    instr->field2 = reg;
                }
                break;

            case ADD: case SUB: 
            case MUL: case DIV:
            case LSHIFT: 
            case RSHIFT:
                reg = PriorityIndex(pri_regs, num_regs, instr->field1) + 1;
                if (reg > (k - 2)) {
                    // reg must be spilled
                    offset = reg;
                    reg = k - 1;
                    instr->field1 = reg;
                    InsertLoadAI(instr, offset, reg, k);
                }
                else {
                    // assign reg a physical register
                    instr->field1 = reg;
                }

                reg = PriorityIndex(pri_regs, num_regs, instr->field2) + 1;
                if (reg > (k - 2)) {
                    // reg needs to be spilled
                    offset = reg;
                    reg = k;
                    instr->field2 = reg;
                    InsertLoadAI(instr, offset, reg, k);
                }
                else {
                    // assign reg a physical register
                    instr->field2 = reg;
                }

                reg = PriorityIndex(pri_regs, num_regs, instr->field3) + 1;
                if (reg > (k - 2)) {
                    // reg needs to be spilled
                    offset = reg;
                    reg = k - 1;
                    instr->field3 = reg;
                    InsertStoreAI(instr, offset, reg, k);
                    instr = instr->next;
                }
                else {
                    // assign reg a physical register
                    instr->field3 = reg;
                    break;
                }

            default:
                break;
        }
        instr = instr->next;
    }
}

// given an array of reigster priorities, the size of the array, and a target register,
// detemerines the index of the target register
int PriorityIndex(int *pri_regs, int num_regs, int reg) {
    for(int i = 0; i < num_regs; i++) {
        if (pri_regs[i] == reg) {
            return i;
        }
    }
    return -1;
}

// given an instruction, returns its MAXLIVE value
int MAXLIVE(Instruction *head) {
    int count = 0;
    if (!head) {
        fprintf(stderr, "error: no instructions given\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < NUM_VR_REGS; i++) {
        if (head->live[i]) {
            count++;
        }
    }
    return count;
}

// given an instruction list and a register number,
// returns 1 if this instruction is the last use of that register
// returns 0 otherwise
int IsLastUse(Instruction *head, int reg) {
    Instruction *instr = NULL;

    if (!head) {
        fprintf(stderr, "error: no instructions given\n");
        exit(EXIT_FAILURE);
    }
    // count the frequencies of each register
    instr = head->next;
    while (instr) {
        switch (instr->opcode) {
            case LOAD: case STORE:
                if ((instr->field1) == reg) {
                    return 0;
                }
                break;

            case ADD: case SUB: 
            case MUL: case DIV:
            case LSHIFT: 
            case RSHIFT:
                if ((instr->field1) == reg || (instr->field2) == reg) {
                    return 0;
                }
                break;

            default:
                break;
        }
        instr = instr->next;
    }
    return 1;
}

// given an instruction list and a list of registers used in the list,
// updates the live range of each intruction
void LiveRange(Instruction *head, int *vregs_freq) {
    if (!head) {
        fprintf(stderr, "error: no instructions given\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < vregs_freq[NUM_VR_REGS]; i++) {
        if (vregs_freq[i]) {
            LiveRangeReg(head, i+1);
        }
    }
}

// given an instruction list and a register, updates the live range for the given register
void LiveRangeReg(Instruction *head, int reg) {
    Instruction *instr = NULL;
    int inuse= 0;

    if (!head) {
        fprintf(stderr, "error: no instructions given\n");
        exit(EXIT_FAILURE);
    }
    // count the frequencies of each register
    instr = head;
    while (instr) {
        switch (instr->opcode) {
            case LOAD: case STORE:
                if ((instr->field2) == reg) {
                    inuse = 1;
                    instr->live[reg-1] = 1;
                    break;
                }
                if (inuse) {
                    if ((instr->field1) == reg) {
                        if (IsLastUse(instr, reg)) {
                            instr->live[reg-1] = 0;
                            return;
                        }
                    }
                    instr->live[reg-1]++;
                    break;
                }
                break;
            case LOADI:
                if ((instr->field2) == reg) {
                    inuse = 1;
                    instr->live[reg-1] = 1;
                    break;
                }
                if (inuse) {
                    instr->live[reg-1] = 1;
                }
                break;

            case ADD: case SUB: 
            case MUL: case DIV:
            case LSHIFT: 
            case RSHIFT:
                if ((instr->field3) == reg) {
                    // reg is the target register
                    inuse = 1;
                    instr->live[reg-1] = 1;
                    break;
                }
                if (inuse) {
                    if ((instr->field1) == reg || (instr->field2) == reg) {
                        // reg is a LHS register
                        if (IsLastUse(instr, reg)) {
                            instr->live[reg-1] = 0;;
                            return;
                        }
                    }
                    // reg is in use and reg is not used or defined
                    instr->live[reg-1] = 1;
                }
                break;

            default:
                break;
        }
        instr = instr->next;
    }
}

int LiveRangeLength(Instruction *head, int reg) {
    Instruction *instr;
    int length = 0;
    if (!head) {
        fprintf(stderr, "error: no instructions given\n");
        exit(EXIT_FAILURE);
    }
    instr = head;
    while (instr) {
        for (int i = 0; i < NUM_VR_REGS; i++) {
            if (instr->live[i] && (i+1) == reg) {
                length++;
                break;
            }
        }
        instr = instr->next;
    }
    return length;
}

// returns register with maximum live length
int MaxLiveRange(Instruction *head , int *pri_regs, int num_regs, int k) {
    int reg = 0, max_length = 0, length = 0;
    if (!head) {
        fprintf(stderr, "error: no instructions given\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < NUM_VR_REGS; i++) {
        if (head->live[i]) {
            length = LiveRangeLength(head, i+1);
            if (length > max_length) {
                max_length = length;
                reg = i+1;
            }
        }
    }
    return reg;
}

// given an instruction list, spills all occurrences of a given register and marks it as not live
int SpillReg(Instruction *head, int reg, int k) {
    Instruction *instr = NULL;
    if (!head) {
        fprintf(stderr, "error: no instructions given\n");
        exit(EXIT_FAILURE);
    }
    instr = head;
    while (instr) {
        switch (instr->opcode) {
            case LOAD: case STORE:
                if (instr->field1 == reg) {
                    InsertLoadAI(instr, k-1, reg, k);
                    instr->live[reg-1] = 0;
                }
                if (instr->field2 == reg) {
                    InsertStoreAI(instr, k , reg, k);
                    instr->live[reg-1] = 0;
                }
                break;

            case LOADI:
                if (instr->field2 == reg) {
                    InsertStoreAI(instr, k , reg, k);
                    instr->live[reg-1] = 0;
                }
                break;

            case ADD: case SUB: 
            case MUL: case DIV:
            case LSHIFT:
            case RSHIFT:
                if (instr->field1 == reg) {
                    InsertStoreAI(instr, k-1 , reg, k);
                    instr->live[reg-1] = 0;
                }
                if (instr->field2 == reg) {
                    InsertStoreAI(instr, k , reg, k);
                    instr->live[reg-1] = 0;
                }
                if (instr->field3 == reg) {
                    InsertStoreAI(instr, k-1 , reg, k);
                    instr->live[reg-1] = 0;
                }
                break;

            default:
                break;

        }
        instr = instr->next;
    }

    return 0;
}

// given a list of live registers and a list of which registers can be allocated/spilled,
// returns a physical register that is not live
int unusedPhsyicalReg(int *live, int *pri_regs, int  k) {
    int reg = 0;
    for (int i = 0; i < (k - 2); i++) { 
        // only consider the first (k-2) entries in pri_regs, as those are the physical registers available
        for (int j = 0; j < NUM_VR_REGS; j++) {
            // pri_regs[i] corresponds to a register r_[pri_regs[i]] that should be allocated to a physical register, r_[i+1]
            // j corresponds to a register r_[j+1]
            // if live[j] == 1, then r_[j+1] is live, else, r_[j+1] is not live
            if (pri_regs[i] == j && !live[j]) {
                reg = j+1;
                break;
            }
        }
    }
    return reg;
}

void MaxLiveAllocSpill(Instruction *head, int *pri_regs, int num_regs, int k) {
    Instruction *instr = NULL;
    int maxlive = 0;
    if (!head) {
        fprintf(stderr, "error: no instructions given\n");
        exit(EXIT_FAILURE);
    }
    instr = head;
    while (instr) {
        maxlive = MAXLIVE(instr);
        if (maxlive > (k - 2)) {
            // spill the register with the longest liverange

        }
        instr = instr->next;
    }
}

void MaxLiveAllocPhysical(Instruction *head, int *pri_regs, int num_regs, int k) {
    int reg = 0;
    Instruction *instr = NULL; 
    if (!head) {
        fprintf(stderr, "error: no instructions given\n");
        exit(EXIT_FAILURE);
    }
    instr = head;
    while (instr) {
        switch (instr->opcode) {
            case LOAD: case STORE:
                reg = PriorityIndex(pri_regs, num_regs, instr->field1) + 1;
                if (reg > (k - 2)) {
                    // reassign reg to an unused physical register
                    reg = unusedPhsyicalReg(instr->live, pri_regs, k);
                    if (!reg) {
                        fprintf(stderr, "error: no unused phsyical registers\n");
                        exit(EXIT_FAILURE);
                    }
                    pri_regs[reg-1] = instr->field1;
                    instr->field1 = reg;
                }
                else {
                    // assign reg to its physical register
                    instr->field1 = reg;
                }

                reg = PriorityIndex(pri_regs, num_regs, instr->field2) + 1;
                if (reg > (k - 2)) {
                    reg = unusedPhsyicalReg(instr->live, pri_regs, k);
                    if (!reg) {
                        fprintf(stderr, "error: no unused phsyical registers\n");
                        exit(EXIT_FAILURE);
                    }
                    pri_regs[reg-1] = instr->field2;
                    instr->field2 = reg;
                }
                else {
                    instr->field2 = reg;
                }
                break;

            case LOADI:
                if (!instr->field2) {
                    break;
                }
                reg = PriorityIndex(pri_regs, num_regs, instr->field2) + 1;
                if (reg > (k - 2)) {
                    reg = unusedPhsyicalReg(instr->live, pri_regs, k);
                    if (!reg) {
                        fprintf(stderr, "error: no unused phsyical registers\n");
                        exit(EXIT_FAILURE);
                    }
                    pri_regs[reg-1] = instr->field2;
                    instr->field2 = reg;
                }
                else {
                    instr->field2 = reg;
                }
                break;

            case ADD: case SUB: 
            case MUL: case DIV:
            case LSHIFT: 
            case RSHIFT:
                reg = PriorityIndex(pri_regs, num_regs, instr->field1) + 1;
                if (reg > (k - 2)) {
                    reg = unusedPhsyicalReg(instr->live, pri_regs, k);
                    if (!reg) {
                        fprintf(stderr, "error: no unused phsyical registers\n");
                        exit(EXIT_FAILURE);
                    }
                    pri_regs[reg-1] = instr->field1;
                    instr->field1 = reg;
                }
                else {
                    instr->field1 = reg;
                }

                reg = PriorityIndex(pri_regs, num_regs, instr->field2) + 1;
                if (reg > (k - 2)) {
                    reg = unusedPhsyicalReg(instr->live, pri_regs, k);
                    if (!reg) {
                        fprintf(stderr, "error: no unused phsyical registers\n");
                        exit(EXIT_FAILURE);
                    }
                    pri_regs[reg-1] = instr->field2;
                    instr->field2 = reg;
                }
                else {
                    instr->field2 = reg;
                }

                reg = PriorityIndex(pri_regs, num_regs, instr->field3) + 1;
                if (reg > (k - 2)) {
                    reg = unusedPhsyicalReg(instr->live, pri_regs, k);
                    if (!reg) {
                        fprintf(stderr, "error: no unused phsyical registers\n");
                        exit(EXIT_FAILURE);
                    }
                    pri_regs[reg-1] = instr->field3;
                    instr->field3 = reg;
                }
                else {
                    instr->field3 = reg;
                    break;
                }

            default:
                break;
        }
        instr = instr->next;
    }
}

void BottomUpAlloc(Instruction *head, int k) {

}

// spills a given register for an instruction
void InsertLoadAI(Instruction *instr, int offset, int reg, int k) {
    Instruction *load = NULL;
    load = calloc(1, sizeof(Instruction));
    if (!load) { 
        fprintf(stderr, "error: calloc allocation failed\n");
    }
    load->opcode = LOADAI;
    load->field1 = 0;
    load->field2 = (-4) * (offset - (k - 2));
    load->field3 = reg;
    load->prev = instr->prev;
    load->next = instr;
    if (instr->prev) {
        // first instruction in list
        (instr->prev)->next = load;
    }
    instr->prev = load;
}

// spills a given register for an instruction
void InsertStoreAI(Instruction *instr, int offset, int reg, int k) {
    Instruction *store = NULL;
    store = calloc(1, sizeof(Instruction));
    if (!store) { 
        fprintf(stderr, "error: calloc allocation failed\n");
    }
    store->opcode = STOREAI;
    store->field1 = reg;
    store->field2 = 0;
    store->field3 = (-4) * (offset - (k - 2));
    store->prev= instr;
    store->next= instr->next;
    if (instr->next) {
        // last instruction in list
        (instr->next)->prev= store;
    }
    instr->next = store;
}
