#ifndef CPU_H
#define CPU_H

#include <stdbool.h>
#include <stdio.h>

#define RAM_SIZE 262144
#define IVT_START 0x0

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

typedef struct sim{
    cpu cpu;
    enum modes {CONTINUOUS, STEP, EXIT} mode;
    bool running;
} sim;

//NOTE: Must be in the same order as the interrupt table
typedef enum interrupt_type{
    HALT_INTRPT,
    CLOCK_INTRPT
} interrupt_type;

sim* get_sim();
void reset_sim(sim* s);
cpu* init_cpu();
void dump_cpu(cpu cpu);
void run_sim(sim* s);
void encode_file(FILE* fp, cpu* cpu);
void execute_instruction(cpu* cpu, unsigned char* instruction);

#endif