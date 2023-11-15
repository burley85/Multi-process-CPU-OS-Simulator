#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "cpu.h"
#include "helper.h"

cpu init_cpu(){
    cpu c;
    c.rax = 0;
    c.rbx = 0;
    c.rcx = 0;
    c.rdx = 0;
    c.rsi = 0;
    c.rdi = 0;
    c.rbp = 0;
    c.rsp = 0;
    c.r8 = 0;
    c.r9 = 0;
    c.r10 = 0;
    c.r11 = 0;
    c.r12 = 0;
    c.r13 = 0;
    c.r14 = 0;
    c.r15 = 0;
    memset(c.memory, 0, 64);
    return c;
}

void dump_cpu(cpu cpu){
    printf("rax: %llu\n", cpu.rax);
    printf("rbx: %llu\n", cpu.rbx);
    printf("rcx: %llu\n", cpu.rcx);
    printf("rdx: %llu\n", cpu.rdx);
    printf("rsi: %llu\n", cpu.rsi);
    printf("rdi: %llu\n", cpu.rdi);
    printf("rbp: %llu\n", cpu.rbp);
    printf("rsp: %llu\n", cpu.rsp);
    printf("r8: %llu\n", cpu.r8);
    printf("r9: %llu\n", cpu.r9);
    printf("r10: %llu\n", cpu.r10);
    printf("r11: %llu\n", cpu.r11);
    printf("r12: %llu\n", cpu.r12);
    printf("r13: %llu\n", cpu.r13);
    printf("r14: %llu\n", cpu.r14);
    printf("r15: %llu\n", cpu.r15);
    printf("of: %d\n", cpu.of);
    printf("sf: %d\n", cpu.sf);
    printf("zf: %d\n", cpu.zf);
    printf("cf: %d\n", cpu.cf);
    printf("\nmemory:\n");
    for(int i = 0; i < 64; i++){
        if(cpu.memory[i] != 0){
            printf("0x%X: ", i);
            print_bin(&cpu.memory[i], 1);
        }
    }
    printf("\n");
}

//Return the pointer to register encoded in register_encoding
unsigned long long* decode_register(cpu* cpu, char register_encoding){
    switch(register_encoding){
        case 0b0000: return &(cpu->rax);
        case 0b0001: return &(cpu->rbx);
        case 0b0010: return &(cpu->rcx);
        case 0b0011: return &(cpu->rdx);
        case 0b0100: return &(cpu->rsi);
        case 0b0101: return &(cpu->rdi);
        case 0b0110: return &(cpu->rbp);
        case 0b0111: return &(cpu->rsp);
        case 0b1000: return &(cpu->r8);
        case 0b1001: return &(cpu->r9);
        case 0b1010: return &(cpu->r10);
        case 0b1011: return &(cpu->r11);
        case 0b1100: return &(cpu->r12);
        case 0b1101: return &(cpu->r13);
        case 0b1110: return &(cpu->r14);
        case 0b1111: return &(cpu->r15);
    }
}

void clear_flags(cpu* cpu){
    cpu->of = false;
    cpu->sf = false;
    cpu->zf = false;
    cpu->cf = false;
}

void execute_add_instruction(cpu* cpu, unsigned long long *operand1, unsigned long long operand2){
    unsigned long long result = *operand1 + operand2;
    
    bool op1_sign = *operand1 >> 63;
    bool op2_sign = operand2 >> 63;
    bool result_sign = result >> 63;

    cpu->of = (op1_sign == op2_sign) && (op1_sign != result_sign);
    cpu->cf = result < *operand1;

    *operand1 = result;
}

void execute_subtraction_instruction(cpu* cpu, unsigned long long *operand1, unsigned long long operand2){
    unsigned long long result = *operand1 - operand2;
    
    bool op1_sign = *operand1 >> 63;
    bool op2_sign = operand2 >> 63;
    bool result_sign = result >> 63;

    cpu->of = (op1_sign != op2_sign) && (op1_sign != result_sign);
    cpu->cf = result > *operand1;

    *operand1 = result;
}

void execute_multiplication_instruction(cpu* cpu, unsigned long long *operand1, unsigned long long operand2){
    long double unsigned_result = *operand1 * operand2;
    long double signed_result = (long long) *operand1 * (long long) operand2;

    cpu->of = signed_result < INT64_MIN || signed_result > INT64_MAX;
    cpu->cf = unsigned_result > UINT64_MAX;

    *operand1 = *operand1 * operand2;
}

void execute_division_instruction(cpu* cpu, unsigned long long *operand1, unsigned long long operand2){
    *operand1 = *operand1 / operand2; //OF and CF are not set by division
}

void execute_assignment_instruction(cpu* cpu, unsigned long long *operand1, unsigned long long operand2){
    *operand1 = operand2; //OF and CF are not set by assignment
}

void execute_load_instruction(cpu* cpu, unsigned long long *operand1, unsigned long long operand2){
    memcpy(operand1, &cpu->memory[operand2], 8); //OF and CF are not set by load instructions
}

void execute_arithmetic_instruction(cpu* cpu, char* instruction){
    char operand1_encoding = instruction[0] & 0b00001111;
    unsigned long long *operand1 = decode_register(cpu, operand1_encoding);
    unsigned long long operand2 = 0;

    if((instruction[1] & 0b11110000) == 0){
        //Operand 2 is a register
        char operand2_encoding = instruction[1] & 0b00001111;
        unsigned long long *temp = decode_register(cpu, operand2_encoding);
        operand2 = *temp;
    } else {
        //Operand 2 is a literal
        char operand2_length = (instruction[1] & 0b11110000) >> 4;
        operand2_length = operand2_length > 8 ? 8 : operand2_length;
        memcpy(&operand2, instruction + 2, operand2_length);
    }

    char operation = instruction[0] >> 4;

    clear_flags(cpu);

    switch(operation){
        case 0b0000:
            execute_add_instruction(cpu, operand1, operand2);
            break;
        case 0b0001:
            execute_subtraction_instruction(cpu, operand1, operand2);
            break;
        case 0b0010:
            execute_multiplication_instruction(cpu, operand1, operand2);
            break;
        case 0b0011:
            execute_division_instruction(cpu, operand1, operand2);
            break;
        case 0b0100:
            execute_assignment_instruction(cpu, operand1, operand2);
            break;
        case 0b0101:
            execute_load_instruction(cpu, operand1, operand2);
            break;
    }
    cpu->zf = *operand1 == 0;
    cpu->sf = *operand1 >> 63;
}

void execute_store_instruction(cpu* cpu, char* instruction){
    unsigned long long operand1 = 0;
    unsigned long long operand2 = 0;
    char operand1Length = 0;

    if(instruction[0] & 0b00001100 == 0){
        //Both operands are registers
        char operand1_encoding = instruction[1] & 0b11110000 >> 4;
        operand1 = *decode_register(cpu, operand1_encoding);
        char operand2_encoding = instruction[1] & 0b00001111;
        operand2 = *decode_register(cpu, operand2_encoding);
    }
    else{
        if(instruction[0] & 0b00001000){
            //Operand 1 is a literal
            operand1Length = ((instruction[0] & 0b00000011) << 1) + ((instruction[1] & 0b10000000) >> 7);
            operand1Length = (operand1Length + 1) * 8;
            memcpy_bits(&operand1, 0, instruction + 1, 4, operand1Length);
            printf("Operand 1: %llu\n", operand1);
            printf("Operand 1 length: %d\n", operand1Length);
        }
        else{
            //Operand 1 is a register
            operand1Length = 4;
            operand1 = *decode_register(cpu, instruction[1] & 0b00001111);
        }
        if(instruction[0] & 0b00000100){
            //Operand 2 is a literal
            char operand2Length = instruction[1] & 0b01110000 >> 4;
            operand2Length = (operand2Length + 1) * 8;
            if(operand1Length == 4) memcpy_bits(&operand2, 0, instruction + 2, 0, operand2Length);
            else memcpy_bits(&operand2, 0, instruction + 1 + (operand1Length / 8), 4, operand2Length);
        }
        else{
            //Operand 2 is a register
            char operand2_encoding = instruction[1 + (operand1Length / 8)] & 0b00001111;
            operand2 = *decode_register(cpu, operand2_encoding);
        }
    }
    printf("Operand 1: %llu\n", operand1);
    printf("Operand 2: %llu\n", operand2);
    memcpy(&cpu->memory[operand1], &operand2, 8);
}

void execute_jump_instruction(cpu* cpu, char* instruction){
    printf("ERROR: Jump instruction not implemented\n");
}

void execute_instruction(cpu* cpu, char* instruction){
    char operation = instruction[0] >> 4;
    if(operation < 0b0110) execute_arithmetic_instruction(cpu, instruction);
    else if(operation == 0b0110) execute_store_instruction(cpu, instruction);
    else execute_jump_instruction(cpu, instruction);
}