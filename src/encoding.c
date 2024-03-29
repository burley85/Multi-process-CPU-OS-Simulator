#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tokenizer.h"
#include "encoding.h"
#include "helper.h"

unsigned char encode_register(int reg){
    switch(reg){
        case RAX: return RAX_ENCODING;
        case RBX: return RBX_ENCODING;
        case RCX: return RCX_ENCODING;
        case RDX: return RDX_ENCODING;
        case RBP: return RBP_ENCODING;
        case RSP: return RSP_ENCODING;
        case RSI: return RSI_ENCODING;
        case RDI: return RDI_ENCODING;
        case R8: return R8_ENCODING;
        case R9: return R9_ENCODING;
        case R10: return R10_ENCODING;
        case R11: return R11_ENCODING;
        case R12: return R12_ENCODING;
        case R13: return R13_ENCODING;
        case R14: return R14_ENCODING;
        case R15: return R15_ENCODING;
        case CR0: return CR0_ENCODING;
        case CR1: return CR1_ENCODING;
        case CR2: return CR2_ENCODING;
        case CR3: return CR3_ENCODING;
        case CR4: return CR4_ENCODING;
        default:
            printf("ERROR: Expected register, got %s\n", token_to_str(reg));
            return HALT_ENCODING;
    }
}

//Returns encoded operator for arithmetic or jump keyword
unsigned char encode_operator(int op){
    switch(op) {
        case PLUS: return ADD_ENCODING;
        case MINUS: return SUB_ENCODING;
        case TIMES: return MUL_ENCODING;
        case DIVIDE: return DIV_ENCODING;
        case EQUALS: return EQUAL_ENCODING;
        default:
            printf("ERROR: Expected operator, got %s\n", token_to_str(op));
            return HALT_ENCODING;
    }
}

unsigned char encode_keyword(int token){
    switch(token){
        case JMP: return JMP_ENCODING;
        case JO: return JO_ENCODING;
        case JNO: return JNO_ENCODING;
        case JZ: return JZ_ENCODING;
        case JNZ: return JNZ_ENCODING;
        case JC: return JC_ENCODING;
        case JNC: return JNC_ENCODING;
        case JS: return JS_ENCODING;
        case JNS: return JNS_ENCODING;
        case HALT: return HALT_ENCODING;
        default:
            printf("ERROR: Expected keyword, got %s\n", token_to_str(token));
            return HALT_ENCODING;
    }
}

//Returns minimum number of bytes needed to store a literal
char literal_length(unsigned long long literal){
    char length = 0;
    do{
        literal >>= 8;
        length++;
    } while(literal > 0);

    if(length > 8){
        printf("ERROR: %llu is too large to store in a literal\n", literal);
        return -1;
    }
    return length;
}

unsigned char* encode_arithmetic_instruction(Parser *p, int operand1, int operator, int* encoding_length){
    int operand2 = p->tokenType;
    unsigned char* encoded_instruction = NULL;

    if(operand2 >= RAX && operand2 <= R15){
        //Instruction encoding will be 2 bytes
        encoded_instruction = malloc(2);
        *encoding_length = 2;
        //Set first 4 bits to the encoded operator
        encoded_instruction[0] = encode_operator(operator) << 4;
        //Set next 4 bits to the encoded register
        encoded_instruction[0] += encode_register(operand1);
        //Set next 4 bits to 0000
        encoded_instruction[1] = 0;
        //Set next 4 bits to the encoded register
        encoded_instruction[1] += encode_register(operand2);
    }

    else if(operand2 == LITERAL){
        int min_length = literal_length(p->lastLiteralValue);
        if(min_length == -1) return NULL;
        //Instruction encoding will be 2 + min_length bytes
        encoded_instruction = malloc(2 + min_length);
        *encoding_length = 2 + min_length;
        //Set first 4 bits to the encoded operator
        encoded_instruction[0] = encode_operator(operator) << 4;
        //Set next 4 bits to the encoded register
        encoded_instruction[0] += encode_register(operand1);
        //Set next 4 bits to length of literal; the next 4 bits will be padding
        encoded_instruction[1] = min_length << 4;
        //Set next min_length bytes to the significant bytes of the literal
        memcpy(encoded_instruction + 2, &(p->lastLiteralValue), min_length);
    }
    else{
        printf("ERROR: Expected register or literal, got %s\n", current_token_str(p));
    }
    return encoded_instruction;
}

unsigned char* encode_load_instruction(Parser *p, int operand1, int* encoding_length){
    int operand2 = next_token(p);
    unsigned char* encoded_instruction = NULL;

    if(operand2 >= RAX && operand2 <= R15){
        //Instruction encoding will be 2 bytes
        encoded_instruction = malloc(2);
        *encoding_length = 2;
        //Set first 4 bits to the load operation encoding
        encoded_instruction[0] = LOAD_ENCODING << 4;
        //Set next 4 bits to the encoded register
        encoded_instruction[0] += encode_register(operand1);
        //Set next 4 bits to 0000
        encoded_instruction[1] = 0;
        //Set next 4 bits to the encoded register
        encoded_instruction[1] += encode_register(operand2);
    }

    else if(operand2 == LITERAL){
        int min_length = literal_length(p->lastLiteralValue);
        if(min_length == -1) return NULL;
        //Instruction encoding will be 2 + min_length bytes
        encoded_instruction = malloc(2 + min_length);
        *encoding_length = 2 + min_length;
        //Set first 4 bits to the load operation encoding
        encoded_instruction[0] = LOAD_ENCODING << 4;
        //Set next 4 bits to the encoded register
        encoded_instruction[0] += encode_register(operand1);
        //Set next 4 bits to length of literal; the next 4 bits will be padding
        encoded_instruction[1] = min_length << 4;
        //Set next min_length bytes to the significant bytes of the literal
        memcpy(encoded_instruction + 2, &(p->lastLiteralValue), min_length);
    }
    else{
        printf("ERROR: Expected register or literal, got %s\n", current_token_str(p));
    }
    return encoded_instruction;
}

unsigned char* encode_store_instruction(Parser *p, int* encoding_length){
    //Operand 1
    int operand1 = next_token(p);
    unsigned long long operand1LiteralVal;
    char operand1Length = 0;
    if(operand1 != LITERAL && (operand1 < RAX || operand1 > R15)){
        printf("ERROR: Expected register or literal, got %s\n", current_token_str(p));
        return NULL;
    }
    if(operand1 == LITERAL){
        operand1LiteralVal = p->lastLiteralValue;
        operand1Length = literal_length(operand1LiteralVal);
    }

    //")="
    if(next_token(p) != RPAREN){
        printf("ERROR: Expected RPAREN, got %s\n", current_token_str(p));
        return NULL;
    }
    if(next_token(p) != EQUALS){
        printf("ERROR: Expected EQUALS, got %s\n", current_token_str(p));
        return NULL;
    }
    
    //Operand 2
    int operand2 = next_token(p);
    unsigned long long operand2LiteralVal;
    char operand2Length = 0;
    if(operand2 != LITERAL && (operand2 < RAX || operand2 > R15)){
        printf("ERROR: Expected register or literal, got %s\n", current_token_str(p));
        return NULL;
    }
    if(operand2 == LITERAL){
        operand2LiteralVal = p->lastLiteralValue;
        operand2Length = literal_length(operand2LiteralVal);
    }
    //Set encoding_length
    *encoding_length = 2 + operand1Length + operand2Length;

    //Both operands are registers
    if(operand1 != LITERAL && operand2 != LITERAL){
        unsigned char* encoded_instruction = malloc(2);
        //Set first 6 bits to 0110 00 for store operation and to signify that both operands are registers
        encoded_instruction[0] = STORE_ENCODING << 4;
        //Set the next byte to encoding each register
        encoded_instruction[1] = encode_register(operand1) << 4;
        encoded_instruction[1] += encode_register(operand2);
        return encoded_instruction;
    }

    //At least one operand is not a register
    int instruction_length_bits = 12; //4 for store operation, 8 for encoding operand types and lengths
    if(operand1 == LITERAL) instruction_length_bits += operand1Length * 8;
    else instruction_length_bits += 4;
    if(operand2 == LITERAL) instruction_length_bits += operand2Length * 8;
    else instruction_length_bits += 4;
    int instruction_length_bytes = instruction_length_bits / 8;
    if(instruction_length_bits % 8 != 0) instruction_length_bytes++;

    unsigned char* encoded_instruction = malloc(instruction_length_bytes);
    //Set first 4 bits to 0110 for store operation
    encoded_instruction[0] = STORE_ENCODING << 4;
    //Set next bit to 1 if operand 1 is a literal
    if(operand1 == LITERAL) encoded_instruction[0] += 1 << 3;
    //Set next bit to 1 if operand 2 is a literal
    if(operand2 == LITERAL) encoded_instruction[0] += 1 << 2;
    //Set next 3 bits to operand 1 length - 1
    operand1Length--;
    memcpy_bits(encoded_instruction, 6, &operand1Length, 5, 3);
    operand1Length++;
    //Set the next 3 bits to operand 2 length - 1
    operand2Length--;
    memcpy_bits(encoded_instruction + 1, 1, &operand2Length, 5, 3); 
    operand2Length++;   
    //Set the next operand1Length bytes to the first operand encoding
    unsigned char* encoded_instruction_pos = encoded_instruction + 1;
    unsigned char encoded_instruction_bit = 4;
    if(operand1 == LITERAL){
        memcpy_bits(encoded_instruction_pos, encoded_instruction_bit, &operand1LiteralVal, 0, operand1Length * 8);
        encoded_instruction_pos += operand1Length;
    }  
    else{
        unsigned char register_encoding = encode_register(operand1);
        encoded_instruction[1] += register_encoding;
        encoded_instruction_bit = 0;
        encoded_instruction_pos++;
    }
    //Set the next operand2Length bytes to the second operand encoding
    if(operand2 == LITERAL){
        memcpy_bits(encoded_instruction_pos, encoded_instruction_bit, &operand2LiteralVal, 0, operand2Length * 8);
    }  
    else{
        unsigned char register_encoding = encode_register(operand2);
        memcpy_bits(encoded_instruction_pos, encoded_instruction_bit, &register_encoding, 4, 4);
    }
    return encoded_instruction;
}   

unsigned char* encode_jump_instruction(Parser *p, int* encoding_length){
    int jump_token = p->tokenType;
    int operand = next_token(p);
    unsigned char* encoded_instruction = NULL;
    if(operand == LABEL){
        //Instruction encoding will be 9 bytes
        encoded_instruction = malloc(9);
        *encoding_length = 9;
        //Set first 4 bits to the jump operation encoding
        encoded_instruction[0] = encode_keyword(jump_token) << 4;
        //Set next 4 bits to 1000 to indicate that operand is a 8 byte literal
        encoded_instruction[0] += 8;
        //Set next 8 bytes to 0, which will be replaced with the address of the label
        memset(encoded_instruction + 1, 0, 8);
    }
    else if(operand == LITERAL){
        unsigned long long operandLiteralVal = p->lastLiteralValue;
        char operandLength = literal_length(operandLiteralVal);
        if(operandLength == -1) return NULL;
        //Instruction encoding will be 1 + operandLength bytes
        encoded_instruction = malloc(1 + operandLength);
        *encoding_length = 1 + operandLength;
        //Set first 4 bits to the jump operation encoding
        encoded_instruction[0] = encode_keyword(jump_token) << 4;
        //Set next 4 bits to length of literal
        encoded_instruction[0] += operandLength;
        //Set next operandLength bytes to the significant bytes of the literal
        memcpy(encoded_instruction + 1, &(p->lastLiteralValue), operandLength);
    }
    else if(operand >= RAX && operand <= R15){
        encoded_instruction = malloc(2);
        *encoding_length = 2;
        //Set first 4 bits to the jump operation 
        encoded_instruction[0] = encode_keyword(jump_token) << 4;
        //Leave the last 4 bits as 0000 to indicate that operand is a register
        //Set next 4 bits to the encoded register
        encoded_instruction[1] = encode_register(operand) << 4;
    }
    else{
        printf("ERROR: Expected register or literal, got %s\n", current_token_str(p));
        return NULL;
    }
    return encoded_instruction;
}

unsigned char* encode_push(Parser *p, int* encoding_length){
    int operand = encode_register(next_token(p));
    if(operand == -1) return NULL;

    *encoding_length = 5;
    unsigned char* encoded_instruction = malloc(5);

    //Encode rsp-8
    encoded_instruction[0] = (SUB_ENCODING << 4) + RSP_ENCODING; // 0001 for -, 0111 for rsp
    encoded_instruction[1] = literal_length(8) << 4; // 0001 for length of operand, 0000 for padding
    encoded_instruction[2] = 8; // 00001000 for 8
    //Encode (rsp)=operand
    encoded_instruction[3] = STORE_ENCODING << 4; // 0110 for store, 00 to signify that both operands are registers, 00 for padding
    encoded_instruction[4] = (RSP_ENCODING << 4) + operand; // 0111 for rsp, next four bits for operand
    return encoded_instruction;
}

unsigned char* encode_pop(Parser *p, int* encoding_length){
    int operand = encode_register(next_token(p));
    if(operand == -1) return NULL;

    *encoding_length = 5;
    unsigned char* encoded_instruction = malloc(5);

    //Encode operand=(rsp)
    encoded_instruction[0] = (LOAD_ENCODING << 4) + operand; // 0101 for load, next four bits for operand
    encoded_instruction[1] = RSP_ENCODING; // 0000 to signify that rsp is a register, 0111 for rsp
    //Encode rsp+8
    encoded_instruction[2] = (ADD_ENCODING << 4) + RSP_ENCODING; // 0000 for +, 0111 for rsp
    encoded_instruction[3] = literal_length(8) << 4; // 0001 for length of operand, 0000 for padding
    encoded_instruction[4] = 8; // 00001000 for 8
    return encoded_instruction;
}

unsigned char* encode_call(Parser *p, int* encoding_length){
    int operand = next_token(p);
    if(operand != LABEL){
        printf("ERROR: Expected label, got %s\n", current_token_str(p));
        return NULL;
    }

    *encoding_length = 10;
    unsigned char* encoded_instruction = malloc(10);

    //Encode pushing rip to stack
    encoded_instruction[0] = PUSH_RIP_ENCODING;

    //Encode unconditional jump to label
    //Set first 4 bits to the jump operation encoding
    encoded_instruction[1] = JMP_ENCODING << 4;
    //Set next 4 bits to 1000 to indicate that operand is a 8 byte literal
    encoded_instruction[1] += 8;
    //Set next 8 bytes to 0, which will be replaced with the address of the label
    memset(encoded_instruction + 2, 0, 8);
    return encoded_instruction;
}

unsigned char* encode_return(Parser *p, int* encoding_length){
    *encoding_length = 1;
    unsigned char* encoded_instruction = malloc(1);
    encoded_instruction[0] = POP_RIP_ENCODING;
    return encoded_instruction;
}

unsigned char* encode_int_return(Parser *p, int* encoding_length){
    *encoding_length = 1;
    unsigned char* encoded_instruction = malloc(1);
    encoded_instruction[0] = IRET_ENCODING;
    return encoded_instruction;
}

unsigned char* encode_halt(Parser *p, int* encoding_length){
    *encoding_length = 1;
    unsigned char* encoded_instruction = malloc(1);
    encoded_instruction[0] = HALT_ENCODING;
    return encoded_instruction;
}

unsigned char* encode_set_control_register(Parser *p, int* encoding_length){
    int control_reg = p->tokenType;
    int operand = next_token(p);    
    int general_reg = next_token(p);

    //Type checking
    if(operand != EQUALS){
        printf("ERROR: Expected EQUALS, got %s\n", current_token_str(p));
        return NULL;
    }
    if(general_reg < RAX || general_reg > R15){
        printf("ERROR: Expected RAX-R15, got %s\n", current_token_str(p));
        return NULL;
    }

    *encoding_length = 2;
    unsigned char* encoded_instruction = malloc(2);
    encoded_instruction[0] = SET_CONTROL_REGISTER_ENCODING;
    encoded_instruction[1] = encode_register(control_reg) << 4;
    encoded_instruction[1] += encode_register(general_reg);

    return encoded_instruction;
}

unsigned char* encode_get_control_register(Parser *p, int general_reg, int* encoding_length){
    int control_reg = p->tokenType;

    *encoding_length = 2;
    unsigned char* encoded_instruction = malloc(2);
    encoded_instruction[0] = GET_CONTROL_REGISTER_ENCODING;
    encoded_instruction[1] = encode_register(general_reg) << 4;
    encoded_instruction[1] += encode_register(control_reg);

    return encoded_instruction;
}

unsigned char* encode_instruction(char *instruction, int* encoding_length) {
    // Find the operation and call the appropriate function
    Parser p;
    p.fileOrString = instruction;
    p.isFile = 0;
    p.position = 0;
    
    int token = next_token(&p);

    if(token == EOF) return NULL;
    if(token == LPAREN) return encode_store_instruction(&p, encoding_length);
    if(token >= JMP && token <= JNS) return encode_jump_instruction(&p, encoding_length);
    if(token >= RAX && token <= R15){
        int reg = token;
        int token1 = next_token(&p);
        int token2 = next_token(&p);
        if(token1 == EQUALS && token2 == LPAREN) return encode_load_instruction(&p, reg, encoding_length);
        if(token1 == EQUALS && token2 >= CR0 && token2 <= CR4) return encode_get_control_register(&p, reg, encoding_length);
        else return encode_arithmetic_instruction(&p, reg, token1, encoding_length);
    }
    if(token >= CR0 && token <= CR4) return encode_set_control_register(&p, encoding_length);
    if(token == POP) return encode_pop(&p, encoding_length);
    if(token == PUSH) return encode_push(&p, encoding_length);
    if(token == HALT) return encode_halt(&p, encoding_length);
    if(token == CALL) return encode_call(&p, encoding_length);
    if(token == RET) return encode_return(&p, encoding_length);
    if(token == IRET) return encode_int_return(&p, encoding_length);
    if(token != LABEL){
        printf("ERROR: Expected register, jump, label, halt, or LPAREN, got %s\n", current_token_str(&p));
    }
    return NULL;
}   

