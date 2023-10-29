#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tokenizer.h"

int current_token(Parser *p){
    return p->tokenType;
}

int parse_jump_token(Parser *p){
    char* jump_keywords[9] = {"jmp", "jo", "jno", "jz", "jnz", "jc", "jnc", "js", "jns"};
    
    char token[5] = "j";
    if(p->isFile) fscanf(p->fileOrString, "%3s", token + 1);
    else sscanf(p->fileOrString + p->position, "%3s", token + 1);

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
    else sscanf(p->fileOrString + p->position, "%3s", token + 1);

    printf("%s\n", token);

    for(int i = 0; i < 9; i++){
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
    else rval = sscanf(p->fileOrString + p->position++, " %c", &c);

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
            break;
        case 'j':
            p->tokenType = parse_jump_token(p);
            break;
        default:
            if(c < '0' || c > '9') break;
            p->tokenType = LITERAL;
            if(p->isFile){
                ungetc(c, p->fileOrString);
                fscanf(p->fileOrString, "%llu", &(p->lastLiteralValue));
            }
            else{
                p->position--;
                int numRead = 0;
                sscanf(p->fileOrString + p->position, "%llu%n", &(p->lastLiteralValue), &numRead);
                p->position += numRead;
            }
            break;
    }
    return p->tokenType;
}