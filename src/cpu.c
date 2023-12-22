#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <Windows.h>

#include "cpu.h"
#include "helper.h"
#include "encoding.h"

#define OBJ_NAME "SimCPUObj"
cpu* init_cpu(){
    int maxSizeHigh = sizeof(cpu) >> 32;
    int maxSizeLow = sizeof(cpu) - maxSizeHigh;
    HANDLE hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, maxSizeHigh, maxSizeLow, OBJ_NAME);

    if(hMapFile == NULL){
        printf("ERROR: Failed to share cpu object\n");
        exit(1);
    }

    cpu* cpuPtr = (cpu*) MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(cpu));
    if(cpuPtr == NULL){
        printf("ERROR: Failed to share cpu object\n");
        exit(1);
    }

    memset(cpuPtr, 0, sizeof(cpu));
    return cpuPtr;
}

cpu* get_cpu(){
    HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, 0, OBJ_NAME);
    if(hMapFile == NULL){
        printf("ERROR: Failed to get shared cpu object\n");
        exit(1);
    }

    cpu* cpuPtr = (cpu*) MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, sizeof(cpu));
    if(cpuPtr == NULL){
        printf("ERROR: Failed to share cpu object\n");
        exit(1);
    }

    return cpuPtr;
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
        case RAX_ENCODING: return &(cpu->rax);
        case RBX_ENCODING: return &(cpu->rbx);
        case RCX_ENCODING: return &(cpu->rcx);
        case RDX_ENCODING: return &(cpu->rdx);
        case RSI_ENCODING: return &(cpu->rsi);
        case RDI_ENCODING: return &(cpu->rdi);
        case RBP_ENCODING: return &(cpu->rbp);
        case RSP_ENCODING: return &(cpu->rsp);
        case R8_ENCODING: return &(cpu->r8);
        case R9_ENCODING: return &(cpu->r9);
        case R10_ENCODING: return &(cpu->r10);
        case R11_ENCODING: return &(cpu->r11);
        case R12_ENCODING: return &(cpu->r12);
        case R13_ENCODING: return &(cpu->r13);
        case R14_ENCODING: return &(cpu->r14);
        case R15_ENCODING: return &(cpu->r15);
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
    unsigned char operand1_encoding = 0;
    memcpy_bits(&operand1_encoding, 4, instruction, 4, 4);
    unsigned long long *operand1 = decode_register(cpu, operand1_encoding);
    unsigned long long operand2 = 0;
    int instruction_length = 2; //The length if operand 2 is a register

    if((instruction[1] >> 4) == 0){
        //Operand 2 is a register
        unsigned char operand2_encoding = 0;
        memcpy_bits(&operand2_encoding, 4, instruction + 1, 4, 4);
        unsigned long long *temp = decode_register(cpu, operand2_encoding);
        operand2 = *temp;
    } else {
        //Operand 2 is a literal
        unsigned char operand2_length = instruction[1] >> 4;
        operand2_length = operand2_length > 8 ? 8 : operand2_length;
        memcpy(&operand2, instruction + 2, operand2_length);
        instruction_length += operand2_length;
    }

    unsigned char operation = instruction[0] >> 4;

    clear_flags(cpu);

    switch(operation){
        case ADD_ENCODING:
            execute_add_instruction(cpu, operand1, operand2);
            break;
        case SUB_ENCODING:
            execute_subtraction_instruction(cpu, operand1, operand2);
            break;
        case MUL_ENCODING:
            execute_multiplication_instruction(cpu, operand1, operand2);
            break;
        case DIV_ENCODING:
            execute_division_instruction(cpu, operand1, operand2);
            break;
        case EQUAL_ENCODING:
            execute_assignment_instruction(cpu, operand1, operand2);
            break;
        case LOAD_ENCODING:
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
    if(!check_bit(instruction[0], 4) && !check_bit(instruction[0], 5)){
        //Both operands are registers
        operand1IsRegister = true;
        unsigned char operand1_encoding = instruction[1] >> 4;
        operand1 = *decode_register(cpu, operand1_encoding);
        unsigned char operand2_encoding = 0;
        memcpy_bits(&operand2_encoding, 4, instruction + 1, 4, 4);
        operand2 = *decode_register(cpu, operand2_encoding);
    }
    else{
        if(check_bit(instruction[0], 4)){
            //Operand 1 is a literal
            memcpy_bits(&operand1Length, 5, instruction, 6, 3);
            operand1Length = (operand1Length + 1) * 8;
            memcpy_bits(&operand1, 0, instruction + 1, 4, operand1Length);
            instructionLength += operand1Length / 8;
        }
        else{
            //Operand 1 is a register
            operand1IsRegister = true;
            operand1Length = 4;
            unsigned char operand1_length = 0;
            memcpy_bits(&operand1_length, 4, instruction + 1, 4, 4);
            operand1 = *decode_register(cpu, operand1);
        }
        if(check_bit(instruction[0], 5)){
            //Operand 2 is a literal
            unsigned char operand2Length = 0;
            memcpy_bits(&operand2Length, 5, instruction + 1, 1, 3);
            operand2Length = (operand2Length + 1) * 8;
            if(operand1IsRegister) memcpy_bits(&operand2, 0, instruction + 2, 0, operand2Length);
            else memcpy_bits(&operand2, 0, instruction + 1 + (operand1Length / 8), 4, operand2Length);
            instructionLength += operand2Length / 8;
        }
        else{
            //Operand 2 is a register
            unsigned char operand2_encoding = 0;
            memcpy_bits(&operand2_encoding, 4, instruction + 1 + (operand1Length / 8), 4, 4);
            operand2 = *decode_register(cpu, operand2_encoding);
        }
    }
    write_memory(cpu, operand1, (unsigned char*) &operand2, operand1IsRegister ? 8 : operand1Length);
    cpu->clock_cycles += CLOCK_CYCLES_PER_WRITE;
    return instructionLength;
}

int execute_jump_instruction(cpu* cpu, unsigned char* instruction){
    unsigned char operation = instruction[0] >> 4;

    unsigned char operand1Length = 0;
    memcpy_bits(&operand1Length, 4, instruction, 4, 4);

    bool condition_met = false;
    switch(operation){
        case JO_ENCODING: //jo
            if(cpu->of) condition_met = true;
            break;
        case JNO_ENCODING: //jno
            if(!cpu->of) condition_met = true;
            break;
        case JZ_ENCODING: //jz
            if(cpu->zf) condition_met = true;
            break;
        case JNZ_ENCODING: //jnz
            if(!cpu->zf) condition_met = true;
            break;
        case JC_ENCODING: //jc
            if(cpu->cf) condition_met = true;
            break;
        case JNC_ENCODING: //jnc
            if(!cpu->cf) condition_met = true;
            break;
        case JS_ENCODING: //js
            if(cpu->sf) condition_met = true;
            break;
        case JNS_ENCODING: //jns
            if(!cpu->sf) condition_met = true;
            break;
        case JMP_ENCODING: //Unconditional jump
            condition_met = true;
            break;
    }
    if(operation != JMP_ENCODING) cpu->clock_cycles += CLOCK_CYCLES_PER_FLAG_CHECK;

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
    unsigned char operation = instruction[0];
    int instruction_length = 0;

    if((operation >> 3) == KERNEL_COMMAND_ENCODING){
        if(operation == PUSH_RIP_ENCODING){
            cpu->rsp -= 8;
            char kernel_command_length = 1;
            if(instruction[1] >> 4 == JMP_ENCODING){
                unsigned char jmp_dest_length = 0;
                memcpy_bits(&jmp_dest_length, 4, instruction + 1, 4, 4);
                if(jmp_dest_length == 0) kernel_command_length += 2;
                else kernel_command_length += 1 + jmp_dest_length;
            }
            else{
                printf("ERROR: Invalid kernel command. Expected JMP after PUSH_RIP\n");
                dump_cpu(*cpu);
                exit(1);
            }
            unsigned long long new_rip = cpu->rip + kernel_command_length;
            write_memory(cpu, cpu->rsp, (unsigned char*) &new_rip, 8);
            instruction_length = 1;
        }
        else if(operation == POP_RIP_ENCODING){
            unsigned long long new_rip;
            unsigned char* mem = read_memory(cpu, cpu->rsp);
            memcpy(&new_rip, mem, 8);
            assign(cpu, &(cpu->rip), new_rip);
            cpu->rsp += 8;
            instruction_length = 0;
        }
        else if(operation == HALT_ENCODING){
            printf("Halting\n");
            dump_cpu(*cpu);
            instruction_length = 1;
        }
        else{
            printf("ERROR: Invalid kernel command\n");
            dump_cpu(*cpu);
        }
    }

    else{
        operation >>= 4;
        if(operation < STORE_ENCODING) instruction_length = execute_arithmetic_instruction(cpu, instruction);
        else if(operation == STORE_ENCODING) instruction_length = execute_store_instruction(cpu, instruction);
        else instruction_length = execute_jump_instruction(cpu, instruction);
    }
    
    cpu->rip += instruction_length;
}

void run_cpu(cpu* cpu){
    unsigned char* instruction = read_memory(cpu, cpu->rip);
    while(*instruction != HALT_ENCODING){
        execute_instruction(cpu, instruction);
        instruction = read_memory(cpu, cpu->rip);
    }
}

//Replaces #define keys with their values, and removes comments
//Returns a pointer to a temporary file containing the preprocessed code
FILE* preprocess_file(FILE* fp){
    FILE* temp = tmpfile();
    if(temp == NULL){
        printf("ERROR: Failed to create temporary file\n");
        exit(1);
    }

    DynamicArray keys = createDynamicArray(0, sizeof(char*), true);
    DynamicArray values = createDynamicArray(0, sizeof(char*), true);

    while(!feof(fp)){
        char* line = fgettrimmedline(fp, NULL);
        if(line == NULL) exit(1);

        //Replace any comments with a null terminator
        char* replace = strchr(line, ';');
        if(replace != NULL){
            replace--;
            while(*replace == ' ' || *replace == '\t' ) replace--;
            *(replace + 1) = '\0';
        }

        //Ignore empty lines
        if(strlen(line) == 0){
            free(line);
            continue;
        }

        //Check if the line is a #define
        if(strncmp(line, "#define ", 8) == 0){
            char* key = malloc(strlen(line) + 1);
            char* value = malloc(strlen(line) + 1);
            memset(key, 0, strlen(line) + 1);
            memset(value, 0, strlen(line) + 1);

            //Get key (format: [A-Za-z_][A-Za-z_0-9]*) and value (rest of line)
            sscanf(line, "#define %[A-Za-z0-9_] %[^\n]", key, value);
            if(strlen(key) == 0 || (key[0] >= '0' && key[0] <= '9')){
                printf("ERROR: Invalid preprocessor command '%s'\n", line);
                exit(1);
            }

            if(appendDynamicArray(&keys, &key) == NULL || appendDynamicArray(&values, &value) == NULL){
                printf("ERROR: Failed to add key-value pair to symbol table\n");
                exit(1);
            }
            free(line);
            continue;
        }
        
        else{
            //Iterate through each word in the line and check if it is a key
            char* str = malloc(strlen(line) + 1);
            memset(str, 0, strlen(line) + 1);
            int str_len = 0;
            int line_pos = 0;

            while(line_pos < strlen(line)){

                char first_char = line[line_pos];
                //If first character is in [A-Za-Z0-9_] then read until next character not in [A-Za-Z0-9_]
                if((first_char >= 'A' && first_char <= 'Z') || (first_char >= 'a' && first_char <= 'z') || first_char == '_'){
                    sscanf(line + line_pos, "%[A-Za-z0-9_]", str);
                    line_pos += strlen(str);
                    
                    //Check if the string is a key (can't be a key if first character is in [0-9])
                    if(!(str[0] >= '0' && str[0] <= '9')){  
                        for(int i = 0; i < keys.size; i++){
                            char* key = *((char**) getDynamicArray(&keys, i));
                            if(strcmp(key, str) == 0){
                                free(str);
                                str = *((char**) getDynamicArray(&values, i));
                                break;
                            }
                        }
                    }
                    //Print the string to the file
                    fprintf(temp, "%s", str);
                }
                //If first character is in [0-9] then read until next character not in [A-Za-z0-9_]
                else if(first_char >= '0' && first_char <= '9'){
                    sscanf(line + line_pos, "%[A-Za-z0-9_]", str);
                    line_pos += strlen(str);

                    //Print the string to the file
                    fprintf(temp, "%s", str);
                }
                //Otherwise, print that character to the file
                else{
                    fprintf(temp, "%c", first_char);
                    line_pos++;
                }
            }
            fprintf(temp, "\n");
            free(str);
        }
    }

    destroyDynamicArray(&keys);
    destroyDynamicArray(&values);
    fclose(fp);
    
    rewind(temp);
    return temp;
}

void encode_file(FILE* fp, cpu* cpu){
    fp = preprocess_file(fp);
    if(fp == NULL){
        printf("ERROR: Failed to preprocess file\n");
        exit(1);
    }

    DynamicArray labels = createDynamicArray(0, sizeof(char*), true);
    DynamicArray label_addresses = createDynamicArray(0, sizeof(unsigned long long), false);

    //Initial pass to look for labels and find their addresses
    unsigned long long address = cpu->mmu.base;
    while(!feof(fp)){
        char* instruction = fgettrimmedline(fp, NULL);
        if(instruction == NULL) exit(1);

        int instruction_length = strlen(instruction);

        //Check if the line is a label (ends with a colon)
        if(instruction[instruction_length - 1] == ':'){
            //Make sure label fits the following regex: [A-Za-z_]+:
            if(instruction_length == 1){
                printf("ERROR: Label cannot be empty\n");
                exit(1);
            }
            for(int i = 0; i < instruction_length - 1; i++){
                if((instruction[i] < 'A' || instruction[i] > 'Z') && (instruction[i] < 'a' || instruction[i] > 'z') && instruction[i] != '_'){
                    printf("ERROR: Label '%s' contains invalid characters\n", instruction);
                    exit(1);
                }
            }

            char* label = malloc(strlen(instruction) + 1);
            memset(label, 0, strlen(instruction) + 1);
            strncpy(label, instruction, instruction_length);
            label[instruction_length - 1] = '\0';
            //Add the label to the symbol table
            if(appendDynamicArray(&labels, &label) == NULL || appendDynamicArray(&label_addresses, &address) == NULL){
                printf("ERROR: Failed to add label to symbol table\n");
                exit(1);
            }
        }
        else{
            int encoding_length;
            unsigned char* encoding = encode_instruction(instruction, &encoding_length);
            address += encoding_length;
            if(encoding != NULL) free(encoding);
        }
    }

    fseek(fp, 0, SEEK_SET);
    address = 0;

    while(!feof(fp)){
        char* instruction = fgettrimmedline(fp, NULL);
        int encoding_length = 0;

        unsigned char* encoding = encode_instruction(instruction, &encoding_length);
        if(encoding != NULL){
            //If instruction is a jump or a call, replace the 8 0 bytes with the address of the label
            bool instruction_is_jump = ((encoding[0] >> 4) >= JMP_ENCODING && (encoding[0] >> 4) <= JNS_ENCODING && !(encoding[0] >> 3 == KERNEL_COMMAND_ENCODING));
            bool instruction_is_call = (encoding[0] == PUSH_RIP_ENCODING) && (encoding[1] >> 4) == JMP_ENCODING;
            if(instruction_is_jump || instruction_is_call){
                //Find the label
                char* label = malloc(strlen(instruction) + 1);
                memset(label, 0, strlen(instruction) + 1);
                sscanf(instruction, "%*s %s", label);
                int i;
                for(i = 0; i < labels.size; i++){
                    char* label_test = *((char**) getDynamicArray(&labels, i));

                    if(strcmp(label_test, label) == 0){
                        unsigned long long label_address = *((unsigned long long*) getDynamicArray(&label_addresses, i));
                        if(instruction_is_jump) memcpy(encoding + 1, &label_address, 8);
                        else if(instruction_is_call) memcpy(encoding + 2, &label_address, 8);
                        break;
                    }
                }
                if(i == labels.size){
                    printf("ERROR: Label %s not found\n", label);
                    return;
                }
            }
            write_memory(cpu, address, encoding, encoding_length);
            address += encoding_length;
            free(encoding);
        }
    }
    unsigned char halt = HALT_ENCODING;
    write_memory(cpu, address, &halt, 1);

    //Free labels
    destroyDynamicArray(&labels);
    destroyDynamicArray(&label_addresses);
}