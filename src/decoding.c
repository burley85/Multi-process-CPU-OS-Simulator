#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "encoding.h"
#include "decoding.h"
#include "symbol_map.h"

char* decode_control_register(unsigned char register_encoding){
    register_encoding = last_n_bits(register_encoding, 4);

    switch(register_encoding){
        case CR0_ENCODING: return "cr0";
        case CR1_ENCODING: return "cr1";
        case CR2_ENCODING: return "cr2";
        case CR3_ENCODING: return "cr3";
        case CR4_ENCODING: return "cr4";
        default: return NULL;
    }
}

char* decode_register(unsigned char register_encoding){
    register_encoding = last_n_bits(register_encoding, 4);

    switch(register_encoding){
        case RAX_ENCODING: return "rax";
        case RBX_ENCODING: return "rbx";
        case RCX_ENCODING: return "rcx";
        case RDX_ENCODING: return "rdx";
        case RSI_ENCODING: return "rsi";
        case RDI_ENCODING: return "rdi";
        case RBP_ENCODING: return "rbp";
        case RSP_ENCODING: return "rsp";
        case R8_ENCODING: return "r8";
        case R9_ENCODING: return "r9";
        case R10_ENCODING: return "r10";
        case R11_ENCODING: return "r11";
        case R12_ENCODING: return "r12";
        case R13_ENCODING: return "r13";
        case R14_ENCODING: return "r14";
        case R15_ENCODING: return "r15";
        default: return NULL;
    }
}

char* ull_to_str(unsigned long long ll){
    char* str = malloc(21);
    memset(str, 0, 21);
    sprintf(str, "%llu", ll);
    return str;
}

char* decode_literal(unsigned char* encoding, int encoding_bytes){
    unsigned long long literal = 0;
    memcpy(&literal, encoding, encoding_bytes);

    return ull_to_str(literal);
}

char* decode_address(unsigned long long address, symbol_map symbols){
    for(int i = 0; i < symbols.label_count; i++){
        if(symbols.label_addresses[i] == address) return symbols.labels[i];
    }
    return NULL;
}

char* decode_arithmetic_instruction(unsigned char* encoded_instruction, int* encoding_length){
    char operator;
    switch(first_n_bits(encoded_instruction[0], 4)){
        case ADD_ENCODING: operator = '+'; break;
        case SUB_ENCODING: operator = '-'; break;
        case MUL_ENCODING: operator = '*'; break;
        case DIV_ENCODING: operator = '/'; break;
        case EQUAL_ENCODING: operator = '='; break;
    }

    char* operand1 = decode_register(last_n_bits(encoded_instruction[0], 4));

    char* operand2;
    bool operand2_is_register = true;

    if(first_n_bits(encoded_instruction[1], 4) == 0){
        operand2 = decode_register(last_n_bits(encoded_instruction[1], 4));
        *encoding_length = 2;
    }
    else{
        operand2_is_register = false;
        char operand2Length = first_n_bits(encoded_instruction[1], 4);
        if(operand2Length > 8) operand2Length = 8;
        
        unsigned long long operand2Value = 0;
        memcpy(&operand2Value, encoded_instruction + 2, operand2Length);
        operand2 = ull_to_str(operand2Value);
        *encoding_length = 2 + operand2Length;
    }

    char* instruction = malloc(strlen(operand1) + strlen(operand2) + 4);
    sprintf(instruction, "%s %c %s", operand1, operator, operand2);

    if(!operand2_is_register) free(operand2);

    return instruction;
}

char* decode_load_instruction(unsigned char* encoded_instruction, int* encoding_length){
    char* operand1 = decode_register(last_n_bits(encoded_instruction[0], 4));

    char* operand2;
    bool operand2_is_register = true;

    if(first_n_bits(encoded_instruction[1], 4) == 0){
        operand2 = decode_register(last_n_bits(encoded_instruction[1], 4));
        *encoding_length = 2;
    }
    else{
        operand2_is_register = false;
        char operand2Length = first_n_bits(encoded_instruction[1], 4);
        if(operand2Length > 8) operand2Length = 8;
        operand2 = decode_literal(encoded_instruction + 2, operand2Length);
        *encoding_length = 2 + operand2Length;
    }

    char* instruction = malloc(strlen(operand1) + strlen(operand2) + 6);
    sprintf(instruction, "%s = (%s)", operand1, operand2);

    if(!operand2_is_register) free(operand2);

    return instruction;
    
}

char* decode_store_instruction(unsigned char* encoded_instruction, int* encoding_length){
    char* operand1;
    bool operand1_is_register = !check_bit(encoded_instruction[0], 4);
    char* operand2;
    bool operand2_is_register = !check_bit(encoded_instruction[0], 5);

    if(operand1_is_register && operand2_is_register){
        operand1 = decode_register(first_n_bits(encoded_instruction[1], 4));
        operand2 = decode_register(last_n_bits(encoded_instruction[1], 4));
        *encoding_length = 2;
    }
    else{
        char operand1Length = 0;
        if(!operand1_is_register){
            memcpy_bits(&operand1Length, 5, encoded_instruction, 6, 3);
            operand1Length++;
            unsigned long long operand1Value = 0;
            memcpy_bits(&operand1Value, 0, encoded_instruction + 1, 4, operand1Length * 8);
            operand1 = ull_to_str(operand1Value);
        }
        else{
            operand1 = decode_register(last_n_bits(encoded_instruction[1], 4));
        }
        char operand2Length = 0;
        if(!operand2_is_register){
            memcpy_bits(&operand2Length, 5, encoded_instruction + 1, 1, 3);
            operand2Length++;
            unsigned long long operand2Value = 0;
            if(operand1_is_register) memcpy(&operand2Value, encoded_instruction + 2, operand2Length);
            else memcpy_bits(&operand2Value, 0, encoded_instruction + 1 + operand1Length, 4, operand2Length * 8);
            operand2 = ull_to_str(operand2Value);
        }
        else{
            operand2 = decode_register(last_n_bits(encoded_instruction[1], 4));
        }
        *encoding_length = 2 + operand1Length + operand2Length;
    }


    char* instruction = malloc(strlen(operand1) + strlen(operand2) + 6);
    sprintf(instruction, "(%s) = %s", operand1, operand2);

    if(!operand1_is_register) free(operand1);
    if(!operand2_is_register) free(operand2);

    return instruction;
}

char* decode_jump_instruction(unsigned char* encoded_instruction, int* encoding_length, symbol_map symbols){
    char* jump_type = "";
    switch(first_n_bits(encoded_instruction[0], 4)){
        case JMP_ENCODING: jump_type = "jmp"; break;
        case JO_ENCODING: jump_type = "jo"; break;
        case JNO_ENCODING: jump_type = "jno"; break;
        case JZ_ENCODING: jump_type = "jz"; break;
        case JNZ_ENCODING: jump_type = "jnz"; break;
        case JC_ENCODING: jump_type = "jc"; break;
        case JNC_ENCODING: jump_type = "jnc"; break;
        case JS_ENCODING: jump_type = "js"; break;
        case JNS_ENCODING: jump_type = "jns"; break;
    }

    char* operand1;
    char operand1Length = last_n_bits(encoded_instruction[0], 4);
    bool free_operand = false;
    if(operand1Length == 0){
        free_operand = true;
        *encoding_length = 2;
        operand1 = decode_register(first_n_bits(encoded_instruction[1], 4));
    }
    else{
        *encoding_length = 1 + operand1Length;

        unsigned long long operand1Value = 0;
        memcpy(&operand1Value, encoded_instruction + 1, operand1Length);

        operand1 = decode_address(operand1Value, symbols);
        if(operand1 == NULL){ //Operand is not a label
            operand1 = malloc(19);
            free_operand = true;
            sprintf(operand1, "0x%llX", operand1Value);
        }
    }

    char* instruction = malloc(strlen(jump_type) + strlen(operand1) + 2);
    sprintf(instruction, "%s %s", jump_type, operand1);
    
    if(free_operand) free(operand1);

    return instruction;
}

char* decode_push_rip_instruction(unsigned char* encoded_instruction, int* encoding_length, symbol_map symbols){
    *encoding_length = 1;
    //Check if instruction is a call (next instruction is a jmp)
    if(first_n_bits(encoded_instruction[1], 4) == JMP_ENCODING){
        char* operand1;
        unsigned long long operand1Value = 0;
        memcpy(&operand1Value, encoded_instruction + 2, 8);
        *encoding_length = 10;

        operand1 = decode_address(operand1Value, symbols);
        if(operand1 == NULL){ //Address does not correspond to a label
            operand1 = malloc(19);
            sprintf(operand1, "0x%llX", operand1Value);
            char* instruction = malloc(strlen(operand1) + 6);
            sprintf(instruction, "call %s", operand1);
            free(operand1);
            return instruction;
        }
        else{
            char* instruction = malloc(strlen(operand1) + 6);
            sprintf(instruction, "call %s", operand1);
            return instruction;
        }
    }
    else{
        char* instruction = malloc(9);
        strcpy(instruction, "push rip");
        *encoding_length = 1;
        return instruction;
    }
}

char* decode_set_control_register(unsigned char* encoded_instruction, int* encoding_length){
    *encoding_length = 2;
    char* instruction = malloc(10);

    char* operand1 = decode_control_register(first_n_bits(encoded_instruction[1], 4));
    char* operand2 = decode_register(last_n_bits(encoded_instruction[1], 4));

    sprintf(instruction, "%s = %s", operand1, operand2);
    return instruction;
}

char* decode_get_control_register(unsigned char* encoded_instruction, int* encoding_length){
    *encoding_length = 2;
    char* instruction = malloc(10);

    char* operand1 = decode_register(first_n_bits(encoded_instruction[1], 4));
    char* operand2 = decode_control_register(last_n_bits(encoded_instruction[1], 4));

    sprintf(instruction, "%s = %s", operand1, operand2);
    return instruction;
}

char* decode_instruction(unsigned char* encoded_instruction, int* encoding_length, symbol_map symbols){
    char* decoding;
    if(encoded_instruction[0] >= KERNEL_COMMAND_ENCODING){
        *encoding_length = 1;
        decoding = malloc(5);
        switch(encoded_instruction[0]){
            case PUSH_RIP_ENCODING: decoding = decode_push_rip_instruction(encoded_instruction, encoding_length, symbols); break;
            case POP_RIP_ENCODING: strcpy(decoding, "ret"); break;
            case HALT_ENCODING: strcpy(decoding, "halt"); break;
            case GET_CONTROL_REGISTER_ENCODING: decoding = decode_get_control_register(encoded_instruction, encoding_length); break;
            case SET_CONTROL_REGISTER_ENCODING: decoding = decode_set_control_register(encoded_instruction, encoding_length); break;
            case IRET_ENCODING: strcpy(decoding, "iret"); break;
        }
    }
    else{
        switch(first_n_bits(encoded_instruction[0], 4)){
            case ADD_ENCODING:
            case SUB_ENCODING:
            case MUL_ENCODING:
            case DIV_ENCODING:
            case EQUAL_ENCODING:
                decoding = decode_arithmetic_instruction(encoded_instruction, encoding_length);
                break;
            case LOAD_ENCODING:
                decoding = decode_load_instruction(encoded_instruction, encoding_length);
                break;
            case STORE_ENCODING:
                decoding = decode_store_instruction(encoded_instruction, encoding_length);
                break;
            default:
                decoding = decode_jump_instruction(encoded_instruction, encoding_length, symbols);
                break;
        }
    }
    return decoding;
}