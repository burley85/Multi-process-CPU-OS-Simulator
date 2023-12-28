#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "encoding.h"
#include "helper.h"

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
    }
}

char* decode_literal(unsigned char* encoding, int encoding_bytes){
    unsigned long long literal = 0;
    memcpy(&literal, encoding, encoding_bytes);

    char* literal_string = malloc(21);
    memset(literal_string, 0, 21);
    sprintf(literal_string, "%llu", literal);
    return literal_string;
}

char* decode_arithmetic_instruction(unsigned char* encoded_instruction){
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
    }
    else{
        operand2_is_register = false;
        char operand2Length = first_n_bits(encoded_instruction[1], 4);
        if(operand2Length > 8) operand2Length = 8;
        operand2 = decode_literal(encoded_instruction + 2, operand2Length);
    }

    char* instruction = malloc(strlen(operand1) + strlen(operand2) + 4);
    sprintf(instruction, "%s %c %s", operand1, operator, operand2);

    if(!operand2_is_register) free(operand2);

    return instruction;
}

char* decode_load_instruction(unsigned char* encoded_instruction){
    
}

char* decode_store_instruction(unsigned char* encoded_instruction){
    
}

char* decode_jump_instruction(unsigned char* encoded_instruction){
    
}

char* decode_instruction(unsigned char* encoded_instruction){
    if(first_n_bits(encoded_instruction[0], 5) == KERNEL_COMMAND_ENCODING){

    }

    else{
        char instruction_type = first_n_bits(encoded_instruction[0], 4);
        switch(first_n_bits(encoded_instruction[0], 4)){
            case ADD_ENCODING:
            case SUB_ENCODING:
            case MUL_ENCODING:
            case DIV_ENCODING:
            case EQUAL_ENCODING:
                return decode_arithmetic_instruction(encoded_instruction);
            case LOAD_ENCODING: return decode_load_instruction(encoded_instruction);
            case STORE_ENCODING: return decode_store_instruction(encoded_instruction);
            default: return decode_jump_instruction(encoded_instruction);
        }
    }

}