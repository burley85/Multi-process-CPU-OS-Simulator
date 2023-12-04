#ifndef TOKENIZER_H
#define TOKENIZER_H

//Registers
#define RAX 0
#define RBX 1
#define RCX 2
#define RDX 3
#define RSI 4
#define RDI 5
#define RBP 6
#define RSP 7
#define R8 8
#define R9 9
#define R10 10
#define R11 11
#define R12 12
#define R13 13
#define R14 14
#define R15 15

//Symbols
#define PLUS 16
#define MINUS 17
#define TIMES 18
#define DIVIDE 19
#define EQUALS 20
#define LPAREN 21
#define RPAREN 22

//Keywords
#define JMP 23
#define JO 24
#define JNO 25
#define JZ 26
#define JNZ 27
#define JC 28
#define JNC 29
#define JS 30
#define JNS 31
#define PUSH 32
#define POP 33
#define HALT 34
#define SYSTEM 35
#define CALL 36
#define RET 37

#define LITERAL 36
#define LABEL 37

#define KEYWORD_LIST {"rax", "rbx", "rcx", "rdx", "rsi", "rdi", "rbp", "rsp", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15", "jmp", "jo", "jno", "jz", "jnz", "jc", "jnc", "js", "jns", "push", "pop", "halt", "system", "call", "ret"}
#define KEYWORD_TOKEN_LIST {RAX, RBX, RCX, RDX, RSI, RDI, RBP, RSP, R8, R9, R10, R11, R12, R13, R14, R15, JMP, JO, JNO, JZ, JNZ, JC, JNC, JS, JNS, PUSH, POP, HALT, SYSTEM, CALL, RET}

typedef struct Parser {
    void* fileOrString;
    int isFile;
    int position;
    int tokenType;
    unsigned long long lastLiteralValue;
} Parser;

char* token_to_str(int token);
char* current_token_str(Parser *p);
int current_token(Parser *p);
int next_token(Parser *p);

#endif