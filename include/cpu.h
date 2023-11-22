#include <stdbool.h>

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

    char memory[64];

    unsigned long long clock_cycles;
} cpu;

cpu init_cpu();
void dump_cpu(cpu cpu);
void encode_file(FILE* fp, cpu* cpu, unsigned long long base);
void execute_instruction(cpu* cpu, char* instruction);
