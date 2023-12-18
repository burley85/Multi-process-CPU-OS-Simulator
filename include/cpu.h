#ifndef CPU_H
#define CPU_H

#include <stdbool.h>
#include <stdio.h>

#define RAM_SIZE 1024

typedef struct mmu{
    unsigned long long base;
    unsigned long long limit;
} mmu;

typedef struct cpu{
    unsigned long long rax;
    unsigned long long rbx;
    unsigned long long rcx;
    unsigned long long rdx;
    unsigned long long rsi;
    unsigned long long rdi;
    unsigned long long rbp;
    unsigned long long rsp;
    unsigned long long r8;
    unsigned long long r9;
    unsigned long long r10;
    unsigned long long r11;
    unsigned long long r12;
    unsigned long long r13;
    unsigned long long r14;
    unsigned long long r15;

    bool of;
    bool sf;
    bool zf;
    bool cf;

    unsigned long long rip;

    unsigned char memory[RAM_SIZE];
    mmu mmu;

    unsigned long long clock_cycles;
} cpu;

cpu init_cpu();
void dump_cpu(cpu cpu);
void run_cpu(cpu* cpu);
void encode_file(FILE* fp, cpu* cpu);
void execute_instruction(cpu* cpu, unsigned char* instruction);

#endif