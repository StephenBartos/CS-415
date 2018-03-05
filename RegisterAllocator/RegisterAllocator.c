#include <stdio.h>
#include <stdlib.h>
#include "RegisterAllocator.h"
#include "InstrList.h"

int main(int argc, char **argv)
{
    Instruction *instr;
    FILE *fp;

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
    instr = ReadInstructionList(fp);
    if (!instr) {
        fprintf(stderr, "error: no instructions given\n");
        exit(EXIT_FAILURE);
    }
    TopDownAllocSimple(instr, 5);
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
                printf("hello\n");
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
void PrioritySort(int *vregs) {


}
void TopDownAllocSimple(Instruction *head, int k) {
    // frequency counts for virtual registers
    int vregs[256] = {0};
    int num_regs = 0;

    CountFrequencies(head, vregs);
    num_regs = CountRegisters(vregs);

    if (num_regs - 2 <= k)
}

void TopDownAlloc(Instruction * head, int k) {

}

void BottomUpAlloc(Instruction *head, int k) {

}
