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
        case PUSH:
            return "push";
        case POP:
            return "pop";
        case HALT:
            return "halt";
        case SYSTEM:
            return "system";
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
    else if(p->tokenType == LABEL){
        char* label = malloc(64);
        memset(label, 0, 64);
        if(p->isFile) fscanf(p->fileOrString, "%63[a-zA-Z0-9_]", label);
        else sscanf((char*) (p->fileOrString) + p->position, "%63[a-zA-Z0-9_]", label);
        return label;
    }
    return token_to_str(p->tokenType);
}

int current_token(Parser *p){
    return p->tokenType;
}

int check_keywords(Parser *p){
    char* keywords[] = KEYWORD_LIST;
    int tokens[] = KEYWORD_TOKEN_LIST;
    int keyword_count = sizeof(keywords) / sizeof(char*);
    
    // Make sure keywords and tokens are the same length
    if(keyword_count != sizeof(tokens) / sizeof(int)){
        printf("ERROR: KEYWORD_TOKEN_LIST and KEYWORD_LIST are not the same length\n");
        exit(1);
    }

    char str[64] = "";
    if(p->isFile) fscanf(p->fileOrString, "%63[a-zA-Z0-9_]", str);
    else sscanf((char*) (p->fileOrString) + p->position, "%63[a-zA-Z0-9_]", str);

    for(int i = 0; i < keyword_count; i++){
        if(strcmp(str, keywords[i]) == 0){
            if(!p->isFile) p->position += strlen(str);
            return tokens[i];
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
            //unget character and check for keywords
            if(p->isFile) ungetc(c, p->fileOrString);
            else p->position--;
            p->tokenType = check_keywords(p);
    }
    if(p->tokenType == -1) p->tokenType = LABEL;
    return p->tokenType;
}