//Symbols
#define PLUS 0
#define MINUS 1
#define TIMES 2
#define DIVIDE 3
#define EQUALS 4
#define LPAREN 5
#define RPAREN 6

//Keywords
#define JMP 7
#define JO 8
#define JNO 9
#define JZ 10
#define JNZ 11
#define JC 12
#define JNC 13
#define JS 14
#define JNS 15

//Registers
#define RAX 16
#define RBX 17
#define RCX 18
#define RDX 19
#define RSI 20
#define RDI 21
#define RBP 22
#define RSP 23
#define R8 24
#define R9 25
#define R10 26
#define R11 27
#define R12 28
#define R13 29
#define R14 30
#define R15 31

#define LITERAL 32


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