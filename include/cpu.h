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

    union{
        unsigned long long flag_register;
        struct{
            bool of;
            bool sf;
            bool zf;
            bool cf;
            bool kf;
        } flags;
    } flags;
    



    unsigned long long rip;

    unsigned char memory[RAM_SIZE];
    mmu mmu;

    unsigned long long clock_cycles;
    unsigned long long interrupt_clock;
} cpu;

//NOTE: Must be in the same order as the interrupt table
typedef enum interrupt_type{
    HALT_INTRPT,
    CLOCK_INTRPT
} interrupt_type;

cpu* init_cpu();
void dump_cpu(cpu cpu);
unsigned char* read_memory(cpu* cpu, unsigned long long address);
void write_memory(cpu* cpu, unsigned long long address, unsigned char* value, int length);
void execute_instruction(cpu* cpu, unsigned char* instruction);
void execute_current_instruction(cpu* cpu);


#endif