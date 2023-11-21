#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "tokenizer.h"

char* token_to_str(int token){
    switch(token){
        case PLUS:
            return "+";
        case MINUS:
            return "-";
        case TIMES:
            return "*";
        case DIVIDE:
            return "/";
        case EQUALS:
            return "=";
        case LPAREN:
            return "(";
        case RPAREN:
            return ")";
        case JMP:
            return "jmp";
        case JO:
            return "jo";
        case JNO:
            return "jno";
        case JZ:
            return "jz";
        case JNZ:
            return "jnz";
        case JC:
            return "jc";
        case JNC:
            return "jnc";
        case JS:
            return "js";
        case JNS:
            return "jns";
        case RAX:
            return "rax";
        case RBX:
            return "rbx";
        case RCX:
            return "rcx";
        case RDX:
            return "rdx";
        case RSI:
            return "rsi";
        case RDI:
            return "rdi";
        case RBP:
            return "rbp";
        case RSP:
            return "rsp";
        case R8:
            return "r8";
        case R9:
            return "r9";
        case R10:
            return "r10";
        case R11:
            return "r11";
        case R12:
            return "r12";
        case R13:
            return "r13";
        case R14:
            return "r14";
        case R15:
            return "r15";
        case LITERAL:
            return "literal";
        case EOF:
            return "EOF";
        case LABEL:
            return "label";
        default:
            return "ERROR";
    }
}

char* current_token_str(Parser *p){
    if(p->tokenType == LITERAL){
        char* constLiteral = malloc(21);
        sprintf(constLiteral, "%llu", p->lastLiteralValue);
        return constLiteral;
    }
    else return token_to_str(p->tokenType);
}

int current_token(Parser *p){
    return p->tokenType;
}

int parse_jump_token(Parser *p){
    char* jump_keywords[9] = {"jmp", "jo", "jno", "jz", "jnz", "jc", "jnc", "js", "jns"};
    
    char token[5] = "j";
    if(p->isFile) fscanf(p->fileOrString, "%3s", token + 1);
    else sscanf((char*) (p->fileOrString) + p->position, "%3s", token + 1);

    for(int i = 0; i < 9; i++){
        if(strcmp(token, jump_keywords[i]) == 0){
            if(!p->isFile) p->position += strlen(token);
            return JMP + i;
        }
    }
    return -1;
}

int parse_register_token(Parser *p){
    char* register_names[16] = {"rax", "rbx", "rcx", "rdx", "rsi", "rdi", "rbp", "rsp", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"};
    
    char token[5] = "r";
    if(p->isFile) fscanf(p->fileOrString, "%3s", token + 1);
    else sscanf((char*) (p->fileOrString) + p->position, "%3s", token + 1);

    for(int i = 0; i < 16; i++){
        if(strcmp(token, register_names[i]) == 0){
            if(!p->isFile) p->position += strlen(token);
            return RAX + i;
        }
    }
    return -1;
}

int next_token(Parser *p){
    char c;
    p->tokenType = -1;

    int rval = 0;
    if(p->isFile) rval = fscanf(p->fileOrString, " %c", &c);
    else{
        rval = sscanf((char*) (p->fileOrString) + p->position++, "%c", &c);
        while(isspace(c)){
            rval = sscanf((char*) (p->fileOrString) + p->position++, "%c", &c);
        }
    }
    if(rval == EOF) {
        p->tokenType = EOF;
        return EOF;
    }

    switch (c) {
        case '+': 
            p->tokenType = PLUS;
            break;
        case '-':
            p->tokenType = MINUS;
            break;
        case '*':
            p->tokenType = TIMES;
            break;
        case '/':
            p->tokenType = DIVIDE;
            break;
        case '=':
            p->tokenType = EQUALS;
            break;
        case '(':
            p->tokenType = LPAREN;
            break;
        case ')':
            p->tokenType = RPAREN;
            break;
        case 'r':
            p->tokenType = parse_register_token(p);
            if(p->tokenType == -1) p->tokenType = LABEL;
            break;
        case 'j':
            p->tokenType = parse_jump_token(p);
            if(p->tokenType == -1) p->tokenType = LABEL;
            break;
        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
            p->tokenType = LITERAL;
            if(p->isFile){
                ungetc(c, p->fileOrString);
                fscanf(p->fileOrString, "%llu", &(p->lastLiteralValue));
            }
            else{
                p->position--;
                int numRead = 0;
                sscanf((char*) (p->fileOrString) + p->position, "%llu%n", &(p->lastLiteralValue), &numRead);
                p->position += numRead;
            }
            break;
        default: 
            p->tokenType = LABEL;

    }
    return p->tokenType;
}