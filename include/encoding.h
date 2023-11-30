/**

Binary encoding:

Instructions will always be a multiple of 8 bits long. Padding will be added to the end of the
instruction if necessary. Instructions are encoded in this format: <operation> <operand 1> <operand
2 (if applicable)> Literal constants can be up to 64 bits long

Registers are encoded in 4 bits, with the following mapping:
    0000: rax
    0001: rbx
    0010: rcx
    0011: rdx
    0100: rsi
    0101: rdi
    0110: rbp
    0111: rsp
    1000: r8
    1001: r9
    1010: r10
    1011: r11
    1100: r12
    1101: r13
    1110: r14
    1111: r15

Operations are encoded in 4 bits, with the following mapping:
    0000: +
    0001: -
    0010: *
    0011: /
    0100: = 
    0101: =(b)
    0110: (a)=
    0111: jmp
    1000: jo
    1001: jno
    1010: jz (or je)
    1011: jnz (or jne)
    1100: jc (or jb)
    1101: jnc (or jnb)
    1110: js
    1111: jns
    11111XXX: Kernel commands (XXX != 000)

For operations 0000-0101, operand 1 is a register and operand 2 is a register or literal constant.
They are encoded as follows:
    Operation (4 bits)
    Operand 1 (4 bits)
    Operand 2 type/length (4 bits):
        0000: register--4 bits
        0001: constant--8 bits
        0010: constant--16 bits
        0011: constant--24 bits
        0100: constant--32 bits
        0101: constant--40 bits
        0110: constant--48 bits
        0111: constant--56 bits
        1XXX: constant--64 bits (XXX is ignored)
    Padding (0 or 4 bits):
        If operand 2 is a register, this is 0 bits
        If operand 2 is a constant, this is 4 bits
    Operand 2 (4, 8, 16, 24, 32, 48, 56, or 64 bits)

For 0110, operand 1 and operand 2 can be either a register or a literal constant.
It is encoded as follows:
    Operation (4 bits)
    Operand types/lengths (4-8 bits):
        11AAABBB: both operands are constants with lengths determined by AAA and BBB
            AAA: operand 1 length
            BBB: operand 2 length
            000: constant--8 bits
            001: constant--16 bits
            010: constant--24 bits
            011: constant--32 bits
            100: constant--40 bits
            101: constant--48 bits
            110: constant--56 bits
            111: constant--64 bits
        01XXXBBB: operand 1 is a register, operand 2 is a constant with length determined by BBB (XXX is ignored):
            000: constant--8 bits
            001: constant--16 bits
            010: constant--24 bits
            011: constant--32 bits
            100: constant--40 bits
            101: constant--48 bits
            110: constant--56 bits
            111: constant--64 bits
        10AAAXXX: operand 1 is a constant with length determined by AAA, operand 2 is a register (XXX is ignored):
            000: constant--8 bits
            001: constant--16 bits
            010: constant--24 bits
            011: constant--32 bits
            100: constant--40 bits
            101: constant--48 bits
            110: constant--56 bits
            111: constant--64 bits
        00XX: both operands are registers (XX is ignored)
        Operand 1 (4-64 bits)
        Operand 2 (4-64 bits)

For 0111-1111, there is only one operand, which can be either a register or a literal constant.
They are encoded as follows:
    Operation (4 bits)
    Operand type/length (4 bits):
        0000: register--4 bits
        0001: constant--8 bits
        0010: constant--16 bits
        0011: constant--24 bits
        0100: constant--32 bits
        0101: constant--40 bits
        0110: constant--48 bits
        0111: constant--56 bits
        1000: constant--64 bits
        1XXX: used for kernel commands when XXX != 000 (See below)
    Operand (8-64 bits)

Kernel commands:    
    11111111: halt

Aliases:
    These operations do not have a unique encoding, but are encoded using a series of other operations:
    In the following, a must be a register
    push a -> rsp-8;(rsp)=a
    pop a -> a=(rsp);rsp+8


**/
#ifndef ENCODING_H
#define ENCODING_H

#define RAX_ENCODING 0b0000
#define RBX_ENCODING 0b0001
#define RCX_ENCODING 0b0010
#define RDX_ENCODING 0b0011
#define RSI_ENCODING 0b0100
#define RDI_ENCODING 0b0101
#define RBP_ENCODING 0b0110
#define RSP_ENCODING 0b0111
#define R8_ENCODING 0b1000
#define R9_ENCODING 0b1001
#define R10_ENCODING 0b1010
#define R11_ENCODING 0b1011
#define R12_ENCODING 0b1100
#define R13_ENCODING 0b1101
#define R14_ENCODING 0b1110
#define R15_ENCODING 0b1111

#define ADD_ENCODING 0b0000
#define SUB_ENCODING 0b0001
#define MUL_ENCODING 0b0010
#define DIV_ENCODING 0b0011
#define EQUAL_ENCODING 0b0100
#define LOAD_ENCODING 0b0101
#define STORE_ENCODING 0b0110

#define JMP_ENCODING 0b0111
#define JO_ENCODING 0b1000
#define JNO_ENCODING 0b1001
#define JZ_ENCODING 0b1010
#define JNZ_ENCODING 0b1011
#define JC_ENCODING 0b1100
#define JNC_ENCODING 0b1101
#define JS_ENCODING 0b1110
#define JNS_ENCODING 0b1111

#define HALT_ENCODING 0b11111111

char* encode_instruction(char *instruction, int* encoding_length);

#endif