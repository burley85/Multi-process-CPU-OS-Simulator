#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include "cpu.h"
#include "helper.h"
#include "encoding.h"

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
    c.of = false;
    c.sf = false;
    c.zf = false;
    c.cf = false;
    c.rip = 0;
    memset(c.memory, 0, RAM_SIZE);
    c.mmu.base = 0;
    c.mmu.limit = 0;
    c.clock_cycles = 0;
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
    printf("rip: 0x%llx\n", cpu.rip);
    printf("\nmemory:\n");
    for(int i = 0; i < RAM_SIZE; i++){
        if(cpu.memory[i] != 0){
            printf("0x%X: ", i);
            print_bin(&cpu.memory[i], 1);
        }
    }
    printf("clock: %llu\n", cpu.clock_cycles);
    printf("\n");
}

#define CLOCK_CYCLES_PER_READ 12
#define CLOCK_CYCLES_PER_WRITE 12
#define CLOCK_CYCLES_PER_ADD 2
#define CLOCK_CYCLES_PER_SUBTRACT 2
#define CLOCK_CYCLES_PER_MULTIPLY 4
#define CLOCK_CYCLES_PER_DIVIDE 8
#define CLOCK_CYCLES_PER_ASSIGNMENT 1
#define CLOCK_CYCLES_PER_FLAG_CHECK 1

unsigned char* read_memory(cpu* cpu, unsigned long long address){
    cpu->clock_cycles += CLOCK_CYCLES_PER_READ;
    if(address < 0 || address >= cpu->mmu.limit) return NULL;
    return &cpu->memory[address + cpu->mmu.base];
}

void write_memory(cpu* cpu, unsigned long long address, unsigned char* value, int length){
    cpu->clock_cycles += CLOCK_CYCLES_PER_WRITE;
    if(address >= 0 && address < cpu->mmu.limit && address + cpu->mmu.base < RAM_SIZE) 
        memcpy(&cpu->memory[address + cpu->mmu.base], value, length);
}

unsigned long long add(cpu* cpu, unsigned long long operand1, unsigned long long operand2){
    cpu->clock_cycles += CLOCK_CYCLES_PER_ADD;
    return operand1 + operand2;
}

unsigned long long subtract(cpu* cpu, unsigned long long operand1, unsigned long long operand2){
    cpu->clock_cycles += CLOCK_CYCLES_PER_SUBTRACT;
    return operand1 - operand2;
}

unsigned long long multiply(cpu* cpu, unsigned long long operand1, unsigned long long operand2){
    cpu->clock_cycles += CLOCK_CYCLES_PER_MULTIPLY;
    return operand1 * operand2;
}

unsigned long long divide(cpu* cpu, unsigned long long operand1, unsigned long long operand2){
    cpu->clock_cycles += CLOCK_CYCLES_PER_DIVIDE;
    return operand1 / operand2;
}

void assign(cpu* cpu, unsigned long long* operand1, unsigned long long operand2){
    cpu->clock_cycles += CLOCK_CYCLES_PER_ASSIGNMENT;
    *operand1 = operand2;
}

//Return the pointer to register encoded in register_encoding
unsigned long long* decode_register(cpu* cpu, unsigned char register_encoding){
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
    unsigned long long result = add(cpu, *operand1, operand2);
    
    bool op1_sign = *operand1 >> 63;
    bool op2_sign = operand2 >> 63;
    bool result_sign = result >> 63;

    cpu->of = (op1_sign == op2_sign) && (op1_sign != result_sign);
    cpu->cf = result < *operand1;

    assign(cpu, operand1, result);
}

void execute_subtraction_instruction(cpu* cpu, unsigned long long *operand1, unsigned long long operand2){
    unsigned long long result = subtract(cpu, *operand1, operand2);
    
    bool op1_sign = *operand1 >> 63;
    bool op2_sign = operand2 >> 63;
    bool result_sign = result >> 63;

    cpu->of = (op1_sign != op2_sign) && (op1_sign != result_sign);
    cpu->cf = result > *operand1;

    assign(cpu, operand1, result);
}

void execute_multiplication_instruction(cpu* cpu, unsigned long long *operand1, unsigned long long operand2){
    long double unsigned_result = *operand1 * operand2;
    long double signed_result = (long long) *operand1 * (long long) operand2;

    cpu->of = signed_result < INT64_MIN || signed_result > INT64_MAX;
    cpu->cf = unsigned_result > UINT64_MAX;

    assign(cpu, operand1, multiply(cpu, *operand1, operand2));
}

void execute_division_instruction(cpu* cpu, unsigned long long *operand1, unsigned long long operand2){
    unsigned long long result = divide(cpu, *operand1, operand2);
    
    //OF and CF are not set by division
    
    assign(cpu, operand1, result);
}

void execute_assignment_instruction(cpu* cpu, unsigned long long *operand1, unsigned long long operand2){
    assign(cpu, operand1, operand2);
}

void execute_load_instruction(cpu* cpu, unsigned long long *operand1, unsigned long long operand2){
    unsigned char* memPtr = read_memory(cpu, operand2);
    unsigned long long result = (unsigned long long) *memPtr;

    assign(cpu, operand1, result);
}

int execute_arithmetic_instruction(cpu* cpu, unsigned char* instruction){
    unsigned char operand1_encoding = instruction[0] & 0b00001111;
    unsigned long long *operand1 = decode_register(cpu, operand1_encoding);
    unsigned long long operand2 = 0;
    int instruction_length = 2; //The length if operand 2 is a register

    if((instruction[1] & 0b11110000) == 0){
        //Operand 2 is a register
        unsigned char operand2_encoding = instruction[1] & 0b00001111;
        unsigned long long *temp = decode_register(cpu, operand2_encoding);
        operand2 = *temp;
    } else {
        //Operand 2 is a literal
        unsigned char operand2_length = (instruction[1] & 0b11110000) >> 4;
        operand2_length = operand2_length > 8 ? 8 : operand2_length;
        memcpy(&operand2, instruction + 2, operand2_length);
        instruction_length += operand2_length;
    }

    unsigned char operation = instruction[0] >> 4;

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
    return instruction_length;
}

int execute_store_instruction(cpu* cpu, unsigned char* instruction){
    unsigned long long operand1 = 0;
    unsigned long long operand2 = 0;
    unsigned char operand1Length = 0;
    bool operand1IsRegister = false;
    int instructionLength = 2; //The length if both operands are registers
    
    if((instruction[0] & 0b00001100) == 0){
        //Both operands are registers
        operand1IsRegister = true;
        unsigned char operand1_encoding = (instruction[1] & 0b11110000) >> 4;
        operand1 = *decode_register(cpu, operand1_encoding);
        unsigned char operand2_encoding = instruction[1] & 0b00001111;
        operand2 = *decode_register(cpu, operand2_encoding);
    }
    else{
        if(instruction[0] & 0b00001000){
            //Operand 1 is a literal
            operand1Length = ((instruction[0] & 0b00000011) << 1) + ((instruction[1] & 0b10000000) >> 7);
            operand1Length = (operand1Length + 1) * 8;
            memcpy_bits(&operand1, 0, instruction + 1, 4, operand1Length);
            instructionLength += operand1Length / 8;
        }
        else{
            //Operand 1 is a register
            operand1IsRegister = true;
            operand1Length = 4;
            operand1 = *decode_register(cpu, instruction[1] & 0b00001111);
        }
        if(instruction[0] & 0b00000100){
            //Operand 2 is a literal
            unsigned char operand2Length = (instruction[1] & 0b01110000) >> 4;
            operand2Length = (operand2Length + 1) * 8;
            if(operand1IsRegister) memcpy_bits(&operand2, 0, instruction + 2, 0, operand2Length);
            else memcpy_bits(&operand2, 0, instruction + 1 + (operand1Length / 8), 4, operand2Length);
            instructionLength += operand2Length / 8;
        }
        else{
            //Operand 2 is a register
            unsigned char operand2_encoding = instruction[1 + (operand1Length / 8)] & 0b00001111;
            operand2 = *decode_register(cpu, operand2_encoding);
        }
    }
    write_memory(cpu, operand1, (unsigned char*) &operand2, operand1IsRegister ? 8 : operand1Length);
    cpu->clock_cycles += CLOCK_CYCLES_PER_WRITE;
    return instructionLength;
}

int execute_jump_instruction(cpu* cpu, unsigned char* instruction){
    unsigned char operation = instruction[0] >> 4;

    unsigned char operand1Length = instruction[0] & 0b00001111;

    bool condition_met = false;
    switch(operation){
        case 0b1000: //jo
            if(cpu->of) condition_met = true;
            break;
        case 0b1001: //jno
            if(!cpu->of) condition_met = true;
            break;
        case 0b1010: //jz
            if(cpu->zf) condition_met = true;
            break;
        case 0b1011: //jnz
            if(!cpu->zf) condition_met = true;
            break;
        case 0b1100: //jc
            if(cpu->cf) condition_met = true;
            break;
        case 0b1101: //jnc
            if(!cpu->cf) condition_met = true;
            break;
        case 0b1110: //js
            if(cpu->sf) condition_met = true;
            break;
        case 0b1111: //jns
            if(!cpu->sf) condition_met = true;
            break;
        case 0b0111: //Unconditional jump
            condition_met = true;
            break;
    }
    if(operation != 0b0111) cpu->clock_cycles += CLOCK_CYCLES_PER_FLAG_CHECK;

    if(condition_met){
        unsigned long long operand1;
        if(operand1Length == 0){
            //Operand 1 is a register
            unsigned char operand1_encoding = instruction[1] >> 4;
            operand1 = *decode_register(cpu, operand1_encoding);
        }
        else{
            //Operand 1 is a literal
            memcpy(&operand1, instruction + 1, operand1Length);
        }
        cpu->rip = operand1;
        assign(cpu, &(cpu->rip), operand1);
        return 0; 
    }

    if(operand1Length == 0) return 2;
    else return operand1Length + 1;    
}

void execute_instruction(cpu* cpu, unsigned char* instruction){
    unsigned char operation = instruction[0] >> 4;
    int instruction_length = 0;
    if(operation < 0b0110) instruction_length = execute_arithmetic_instruction(cpu, instruction);
    else if(operation == 0b0110) instruction_length = execute_store_instruction(cpu, instruction);
    else instruction_length = execute_jump_instruction(cpu, instruction);
    cpu->rip += instruction_length;
}

void run_cpu(cpu* cpu){
    unsigned char* instruction = read_memory(cpu, cpu->rip);
    while(*instruction != 0b11111111){
        execute_instruction(cpu, instruction);
        instruction = read_memory(cpu, cpu->rip);
    }
}

void encode_file(FILE* fp, cpu* cpu){
    unsigned char* labels[128];
    unsigned long long label_addresses[128];
    int label_count = 0;

    //Initial pass to look for labels and find their addresses
    unsigned long long address = cpu->mmu.base;
    while(!feof(fp)){
        char instruction[128] = "";
        char terminator = '\0';
        fscanf(fp, " %128[^;:\n]%c", &instruction, &terminator);
        if(strlen(instruction) == 0) fscanf(fp, " %c", &terminator);

        //Check if the line is a label (next character is a colon)
        if(terminator == ':'){
            if(strlen(instruction) == 0){
                printf("ERROR: Label cannot be empty\n");
                return;
            }
            char* label = malloc(strlen(instruction) + 1);
            strcpy(label, instruction);
            //Get rid of whitespace at the end of the label
            while(label[strlen(label) - 1] == ' ' || label[strlen(label) - 1] == '\t') label[strlen(label) - 1] = '\0';
            //Add the label to the symbol table
            labels[label_count] = label;
            label_addresses[label_count] = address;
            label_count++;
        }
        else{
            if(terminator == ';') fscanf(fp, "%*[^\n]");
            if(strlen(instruction) == 0) continue;
            int encoding_length;
            unsigned char* encoding = encode_instruction(instruction, &encoding_length);
            address += encoding_length;
            if(encoding != NULL) free(encoding);
        }
    }

    fseek(fp, 0, SEEK_SET);
    address = 0;
    while(!feof(fp)){
        char instruction[128] = "";
        char terminator = '\0';
        fscanf(fp, " %128[^;:\n]%c", &instruction, &terminator);
        if(strlen(instruction) == 0) fscanf(fp, " %c", &terminator);
        if(terminator == ':') continue;
        if(terminator == ';') fscanf(fp, "%*[^\n]");
        int encoding_length = 0;

        unsigned char* encoding = encode_instruction(instruction, &encoding_length);
        if(encoding != NULL){
            //If instruction is a jump, replace the 8 0 bytes with the address of the label
            if(encoding[0] >> 4 > 0b0110){
                //Find the label
                char label_location[128] = "";
                sscanf(instruction, "%*s %s:", label_location);
                int i;
                for(i = 0; i < label_count; i++){
                    if(strcmp(labels[i], label_location) == 0){
                        memcpy(encoding + 1, &label_addresses[i], 8);
                        break;
                    }
                }
                if(i == label_count){
                    printf("ERROR: Label %s not found\n", label_location);
                    return;
                }
            }
            write_memory(cpu, address, encoding, encoding_length);
            address += encoding_length;
            free(encoding);
        }
    }
    unsigned char halt = 0b11111111;
    write_memory(cpu, address,  &halt, 1);

    //Free labels
    for(int i = 0; i < label_count; i++){
        free(labels[i]);
    }
}