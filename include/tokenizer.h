#ifndef TOKENIZER_H
#define TOKENIZER_H

enum TokenType {
    // Registers
    RAX, RBX, RCX, RDX, RSI, RDI, RBP, RSP, R8, R9, R10, R11, R12, R13, R14, R15,

    // Control Registers
    CR0, CR1, CR2, CR3,

    // Symbols
    PLUS, MINUS, TIMES, DIVIDE, EQUALS, LPAREN, RPAREN,

    // Keywords
    JMP, JO, JNO, JZ, JNZ, JC, JNC, JS, JNS, PUSH, POP, HALT, SYSTEM, CALL, RET,

    // Other
    LITERAL, LABEL
};

#define KEYWORD_LIST {"rax", "rbx", "rcx", "rdx", "rsi", "rdi", "rbp", "rsp", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15", "jmp", "jo", "jno", "jz", "jnz", "jc", "jnc", "js", "jns", "push", "pop", "halt", "system", "call", "ret", "cr0", "cr1", "cr2", "cr3"}
#define KEYWORD_TOKEN_LIST {RAX, RBX, RCX, RDX, RSI, RDI, RBP, RSP, R8, R9, R10, R11, R12, R13, R14, R15, JMP, JO, JNO, JZ, JNZ, JC, JNC, JS, JNS, PUSH, POP, HALT, SYSTEM, CALL, RET, CR0, CR1, CR2, CR3}

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