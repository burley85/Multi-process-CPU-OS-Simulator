#ifndef COMPILE_MAIN_EXE
#include <stdio.h>
#include <Windows.h>
#include <string.h>

#include "cpu.h"
#include "decoding.h"
#include "helper.h"

void copy_to_console_buffer(char** buffer, char* formatted_string, ...) {
    va_list args;
    va_start(args, formatted_string);

    vsprintf(*buffer, formatted_string, args);

    va_end(args);
    *buffer += strlen(*buffer);
}

void update_decoded_instructions(cpu* c, char* decoded_instructions[], int instruction_positions[]){
    unsigned long long pos = c->rip;
    int encoding_length;

    if(decoded_instructions[0] != NULL) free(decoded_instructions[0]);
    decoded_instructions[0] = decode_instruction(&(c->memory[pos]), &encoding_length);
    pos += encoding_length;
    instruction_positions[1] = pos - c->rip; //Address of next instruction relative to rip
    if(strlen(decoded_instructions[0]) > 48){ //make last 3 characters "..."
        decoded_instructions[0][45] = '.';
        decoded_instructions[0][46] = '.';
        decoded_instructions[0][47] = '.';
    }

    if(decoded_instructions[1] != NULL) free(decoded_instructions[1]);
    decoded_instructions[1] = decode_instruction(&(c->memory[pos]), &encoding_length);
    pos += encoding_length;
    instruction_positions[2] = pos - c->rip; //Address of next instruction relative to rip
    if(strlen(decoded_instructions[1]) > 48){ //make last 3 characters "..."
        decoded_instructions[1][45] = '.';
        decoded_instructions[1][46] = '.';
        decoded_instructions[1][47] = '.';
    }

    if(decoded_instructions[2] != NULL) free(decoded_instructions[2]);
    decoded_instructions[2] = decode_instruction(&(c->memory[pos]), &encoding_length);
    pos += encoding_length;
    instruction_positions[3] = pos - c->rip; //Address of next instruction relative to rip
    if(strlen(decoded_instructions[2]) > 48){ //make last 3 characters "..."
        decoded_instructions[2][45] = '.';
        decoded_instructions[2][46] = '.';
        decoded_instructions[2][47] = '.';
    }

    if(decoded_instructions[3] != NULL) free(decoded_instructions[3]);
    decoded_instructions[3] = decode_instruction(&(c->memory[pos]), &encoding_length);
    pos += encoding_length;
    instruction_positions[4] = pos - c->rip; //Address of next instruction relative to rip
    if(strlen(decoded_instructions[3]) > 48){ //make last 3 characters "..."
        decoded_instructions[3][45] = '.';
        decoded_instructions[3][46] = '.';
        decoded_instructions[3][47] = '.';
    }

    if(decoded_instructions[4] != NULL) free(decoded_instructions[4]);
    decoded_instructions[4] = decode_instruction(&(c->memory[pos]), &encoding_length);
    pos += encoding_length;
    if(strlen(decoded_instructions[4]) > 48){ //make last 3 characters "..."
        decoded_instructions[4][45] = '.';
        decoded_instructions[4][46] = '.';
        decoded_instructions[4][47] = '.';
    }
}

unsigned long long get_stack_value(cpu* c, long long stack_offset){
    long long mem_position = c->rsp + stack_offset;
    if(mem_position < 0 || mem_position + sizeof(unsigned long long) > RAM_SIZE) return 0;
    return *((unsigned long long*) &(c->memory[mem_position]));
}

//Print cpu information to the console
void update_cpu_buffer(char* buffer, sim* s, char* decoded_instructions[], int instruction_positions[]){
    cpu* cpu = &(s->cpu);
    
    char* line_format =
        "+------------+---------+          +--------------+-----------------------------+          +----------------+----------+\n";
    copy_to_console_buffer(&buffer, line_format);

    char* sim_status = s->running ? "RUNNING" : "PAUSED";
    if(s->mode == EXIT) sim_status = "EXIT";

    unsigned long long sim_runtime = cpu->clock_cycles;
    char* interrupt_type = "NONE";
    line_format = 
        "| SIM STATUS | %7s |          | SIM  RUNTIME | %20llu clocks |          | INTERRUPT TYPE | %8s |\n";
    copy_to_console_buffer(&buffer, line_format, sim_status, sim_runtime, interrupt_type);
    
    line_format =
        "+------------+---------+          +--------------+-----------------------------+          +----------------+----------+\n";
    copy_to_console_buffer(&buffer, line_format);

    line_format =
        "+-----+--------------------+----------------------+-----+--------------------+----------------------+ +-----------+---+\n";
    copy_to_console_buffer(&buffer, line_format);

    line_format =
        "| rax | 0x%-16llx | %20llu | r8  | 0x%-16llx | %20llu | |  OVERFLOW | %d |\n";
    copy_to_console_buffer(&buffer, line_format, cpu->rax, cpu->rax, cpu->r8, cpu->r8, cpu->of);

    line_format =
        "| rbx | 0x%-16llx | %20llu | r9  | 0x%-16llx | %20llu | |  ZERO     | %d |\n";
    copy_to_console_buffer(&buffer, line_format, cpu->rbx, cpu->rbx, cpu->r9, cpu->r9, cpu->zf);

    line_format =
        "| rcx | 0x%-16llx | %20llu | r10 | 0x%-16llx | %20llu | |  SIGN     | %d |\n";
    copy_to_console_buffer(&buffer, line_format, cpu->rcx, cpu->rcx, cpu->r10, cpu->r10, cpu->sf);

    line_format =
        "| rdx | 0x%-16llx | %20llu | r11 | 0x%-16llx | %20llu | |  CARRY    | %d |\n";
    copy_to_console_buffer(&buffer, line_format, cpu->rdx, cpu->rdx, cpu->r11, cpu->r11, cpu->cf);

    line_format =
        "| rsi | 0x%-16llx | %20llu | r12 | 0x%-16llx | %20llu | +-----------+---+\n";
    copy_to_console_buffer(&buffer, line_format, cpu->rsi, cpu->rsi, cpu->r12, cpu->r12);

    line_format =
        "| rdi | 0x%-16llx | %20llu | r13 | 0x%-16llx | %20llu |\n";
    copy_to_console_buffer(&buffer, line_format, cpu->rdi, cpu->rdi, cpu->r13, cpu->r13);

    line_format =
        "| rbp | 0x%-16llx | %20llu | r14 | 0x%-16llx | %20llu |\n";
    copy_to_console_buffer(&buffer, line_format, cpu->rbp, cpu->rbp, cpu->r14, cpu->r14);

    line_format =
        "| rsp | 0x%-16llx | %20llu | r15 | 0x%-16llx | %20llu |\n";
    copy_to_console_buffer(&buffer, line_format, cpu->rsp, cpu->rsp, cpu->r15, cpu->r15);

    line_format =
        "| rip | 0x%-16llx | %20llu | mmu | 0x%-6llx->0x%-6llx |     %7llu->%7llu |\n";
    copy_to_console_buffer(&buffer, line_format, cpu->rip, cpu->rip, cpu->mmu.base, cpu->mmu.limit, cpu->mmu.base, cpu->mmu.limit);

    line_format =
        "+-----+--------------------+----------------------+-----+--------------------+----------------------+\n";
    copy_to_console_buffer(&buffer, line_format);

    line_format =
        "+------------+----------------------------------------------------+ +-------------------+-----------------------------+\n";
    copy_to_console_buffer(&buffer, line_format);

    line_format = 
        "| rip  --->  |   %48s | | INTERRUPT CLOCK   | %20llu clocks |\n";
    copy_to_console_buffer(&buffer, line_format, decoded_instructions[0], 0);

    line_format = 
        "| rip + 0x%-2llx |   %48s | | QUANTUM           | %6llu / %6llu      clocks |\n";
    copy_to_console_buffer(&buffer, line_format, instruction_positions[1], decoded_instructions[1], 0, 0);

    line_format = 
        "| rip + 0x%-2llx |   %48s | | PROC CPU TIME     | %20llu clocks |\n";
    copy_to_console_buffer(&buffer, line_format, instruction_positions[2], decoded_instructions[2], 0, 0);

    line_format = 
        "| rip + 0x%-2llx |   %48s | | PROC ELAPSED TIME | %20llu clocks |\n";
    copy_to_console_buffer(&buffer, line_format, instruction_positions[3], decoded_instructions[3], 0, 0);

    line_format = 
        "| rip + 0x%-2llx |   %48s | | PID               |        %20llu |\n";
    copy_to_console_buffer(&buffer, line_format, instruction_positions[4], decoded_instructions[4], 0, 0);

    line_format =
        "+------------+----------------------------------------------------+ +-------------------+-----------------------------+\n";
    copy_to_console_buffer(&buffer, line_format);

    line_format =
        "+----------+--------------------+----------------------+ +----------+--------------------+----------------------+\n";
    copy_to_console_buffer(&buffer, line_format);

    unsigned long long firstVal = get_stack_value(cpu, -48);
    unsigned long long secondVal = get_stack_value(cpu, -40);
    line_format =
        "| rsp - 48 | 0x%-16llx | %20llu | | rsp - 40 | 0x%-16llx | %20llu |\n";
    copy_to_console_buffer(&buffer, line_format, firstVal, firstVal, secondVal, secondVal);
    
    firstVal = get_stack_value(cpu, -32);
    secondVal = get_stack_value(cpu, -24);
    line_format =
        "| rsp - 32 | 0x%-16llx | %20llu | | rsp - 24 | 0x%-16llx | %20llu |\n";
    copy_to_console_buffer(&buffer, line_format, firstVal, firstVal, secondVal, secondVal);

    firstVal = get_stack_value(cpu, -16);
    secondVal = get_stack_value(cpu, -8);
    line_format =
        "| rsp - 16 | 0x%-16llx | %20llu | | rsp -  8 | 0x%-16llx | %20llu |\n";
    copy_to_console_buffer(&buffer, line_format, firstVal, firstVal, secondVal, secondVal);
    
    firstVal = get_stack_value(cpu, 0);
    secondVal = get_stack_value(cpu, 8);
    line_format =
        "| rsp  ->  | 0x%-16llx | %20llu | | rsp +  8 | 0x%-16llx | %20llu |\n";
    copy_to_console_buffer(&buffer, line_format, firstVal, firstVal, secondVal, secondVal);
    
    firstVal = get_stack_value(cpu, 16);
    secondVal = get_stack_value(cpu, 24);
    line_format =
        "| rsp + 16 | 0x%-16llx | %20llu | | rsp + 24 | 0x%-16llx | %20llu |\n";
    copy_to_console_buffer(&buffer, line_format, firstVal, firstVal, secondVal, secondVal);
    
    firstVal = get_stack_value(cpu, 32);
    secondVal = get_stack_value(cpu, 40);
    line_format =
        "| rsp + 32 | 0x%-16llx | %20llu | | rsp + 40 | 0x%-16llx | %20llu |\n";
    copy_to_console_buffer(&buffer, line_format, firstVal, firstVal, secondVal, secondVal);
    
    firstVal = get_stack_value(cpu, 48);
    secondVal = get_stack_value(cpu, 56);
    line_format =
        "| rsp + 48 | 0x%-16llx | %20llu | | rsp + 56 | 0x%-16llx | %20llu |\n";
    copy_to_console_buffer(&buffer, line_format, firstVal, firstVal, secondVal, secondVal);

    line_format =
        "+----------+--------------------+----------------------+ +----------+--------------------+----------------------+\n";
    copy_to_console_buffer(&buffer, line_format, 0, 0);
}

int main(){
    //Get HANDLE to the console
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    //SetConsoleScreenBufferSize(h, (COORD){200, 200});

    char buffer[40000] = "";
    sim* s = get_sim();
    cpu* cpu = &(s->cpu);

    char* decoded_instructions[5] = {NULL, NULL, NULL, NULL, NULL};
    int instruction_positions[5] = {0, 0, 0, 0, 0};

    while(1){
        update_decoded_instructions(cpu, decoded_instructions, instruction_positions);
        update_cpu_buffer(buffer, s, decoded_instructions, instruction_positions);
        if(s->mode == STEP && !s->running){
            SetConsoleCursorPosition(h, (COORD){0, 0});
            update_decoded_instructions(cpu, decoded_instructions, instruction_positions);
            update_cpu_buffer(buffer, s, decoded_instructions, instruction_positions);
            scanf("%*c");
            s->running = true;
        }
        if(!WriteConsole(h, buffer, strlen(buffer), NULL, NULL)){
            printf("Error: %lu\n", GetLastError());
        }
        //Scroll to the top of the console buffer
        //SetConsoleWindowInfo(h, 
        SetConsoleCursorPosition(h, (COORD){0, 0});
        
    }
}

#endif