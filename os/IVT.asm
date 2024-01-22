;Return the rdi-th bit of rsi
check_bit:
    push r10
    
    ;Multiply rsi by 2^rdi and then divide by 2^63 (9223372036854775808)
    r10 = rsi ;Preserve rsi
    call pow2 ;rax = 2^rdi
    r10 * rax ;Multiply rsi by 2^rdi
    r10 / 9223372036854775808 ;Divide by 2^63
    rax = r10

    pop r10
    ret

;Return 2^rdi
pow2:
    rax = 1
    rdi = rdi
    jnz pow2_loop
    ret
    pow2_loop:
    rax * 2
    rdi - 1
    jnz pow2_loop
    ret

;IVT setup
    ;IVT structure will be stored at 0x00-0x3F
    ;Each interrupt will get 2 bytes to encode address of the interrupt handler

#define IVT_START 0

rsp = 2048
call store_halt_int ;Interrupt 0
call store_clk_int  ;Interrupt 1
halt

get_instruction_address:
    ;Get address of caller by getting address of return address
    rax = (rsp)
    ret

;NOTE: functions that store the interrupt handler address must be directly before the interrupt handler

store_halt_int:
    r8 = IVT_START
    r8 + 0 ;Offset of 0 from IVT_START
    r9 = 5 ;Length of instructions after call get_instruction_address
    call get_instruction_address
    rax + r9
    (r8) = rax
    ret
halt_int_handler:
    ret

store_clk_int:
    r8 = IVT_START
    r8 + 2 ;Offset of 2 from IVT_START
    r9 = 5 ;Length of instructions after call
    call get_instruction_address
    rax + r9
    (r8) = rax
    ret
clk_int_handler:
    ret

;PCB Table layout:
;   First 24 bytes: 
;       1 byte for PID of current process
;       8 bytes storing an array of boolean values, indicating which PCB slots are in use
;       15 bytes for scheduling info
;   Each process's PCB is stored at PCB_TABLE_START + (pid * 24)
;   PID 0 is reserved
;   Max number of processes is 63 (Table size is 64 * 24 bytes)

;Individual PCB layout:
;   1 bytes for priority
;   4 bits for flags, 2 bits for process status, 2 bits unused
;   2 bytes unused
;   4 bytes for total cpu time
;   4 bytes for rax, rbx, rcx, rdi, rsi, rbp, rsp, r8, r9, r10, r11, r12, r13, r14, r15
;   4 bytes for rip
;   4 bytes for base address
;   4 bytes for limit

;Put the pid of the next process at PCB_TABLE_START + 9
;Return 0 if the queue is empty; otherwise, return the pid of the next process

#define PCB_TABLE_START 64
#define PCB_SIZE 24

#define PCB_TABLE_START 64
#define PCB_SIZE 160
#define PCB_RAX_OFFSET 0
#define PCB_RBX_OFFSET 8
#define PCB_RCX_OFFSET 16
#define PCB_RDI_OFFSET 24
#define PCB_RSI_OFFSET 32
#define PCB_RBP_OFFSET 40
#define PCB_RSP_OFFSET 48
#define PCB_R8_OFFSET 56
#define PCB_R9_OFFSET 64
#define PCB_R10_OFFSET 72
#define PCB_R11_OFFSET 80
#define PCB_R12_OFFSET 88
#define PCB_R13_OFFSET 96
#define PCB_R14_OFFSET 104
#define PCB_R15_OFFSET 112
#define PCB_RIP_OFFSET 120
#define PCB_CPU_TIME_OFFSET 128
#define PCB_BASE_ADDR_OFFSET 136
#define PCB_LIMIT_OFFSET 144
#define PCB_FLAGS_OFFSET 152

round_robin_scheduler:
    push r10
    push r11
    push r12

    r10 = (PCB_TABLE_START) ;Current process PID
    r10 / 72057594037927936 ;Isolate first byte

    r11 = PCB_TABLE_START
    r11 + 1
    r11 = (r11) ;Array of boolean values indicating which PCB slots are in use



    ;Look for the next PCB slot that is in use
    r12 = r10 ;r12 is the PCB slot to check
    loop_check_bit:
        r12 + 1
        r12 - 64
        jz loop_check_bit ;r12 was 64, so circle back to the beginning and check slot 1
        r12 + 64

        ;Check if slot r12 is in use
        rdi = r12
        rsi = r11
        call check_bit
        rax = rax
        jnz store_next_pid ;This PCB slot is in use

        r12 - r10
        jz loop_check_bit_end ;No PCB slots are in use, including the current one (empty queue)
        r12 + r10

        store_next_pid:
            ;r12 is the PID of the next process
            ;Store the PID of the next process at PCB_TABLE_START + 9
            r11 = PCB_TABLE_START
            r11 + 9
            (r11) = r12

    round_robin_scheduler_end:
        pop r12
        pop r11
        pop r10
        ret

;Update the PCB of the current process
;Process rip must be stored at (rsp + 24)
;Process rsp must be stored at (rsp + 16)
;Flag register must be stored at (rsp + 8)
;NOTE: This function may clobber all registers
store_PCB:
    push rax ;Preserve rax

    ;Get process's PCB address
    rax = PCB_TABLE_START
    rax = (rax) ;Current process's PID
    rax * PCB_SIZE
    rax + PCB_TABLE_START ;Address of current process's PCB

    ;Store rbx
    rax + PCB_RBX_OFFSET ;Address of PCB rbx
    (rax) = rbx

    ;Store rax
    rbx = rax
    rbx - PCB_RBX_OFFSET
    rbx + PCB_RAX_OFFSET ;Address of PCB rax
    pop rax ;Restore process's rax
    (rbx) = rax

    ;Store rcx-rbp
    rbx - PCB_RAX_OFFSET
    rbx + PCB_RCX_OFFSET
    (rbx) = rcx
    rbx + 8
    (rbx) = rdi
    rbx + 8
    (rbx) = rsi
    rbx + 8
    (rbx) = rbp
    rbx - PCB_RBP_OFFSET ;Reset rbx to point to start of PCB

    ;Store r8-r15
    rbx + 8
    (rbx) = r8
    rbx + 8
    (rbx) = r9
    rbx + 8
    (rbx) = r10
    rbx + 8
    (rbx) = r11
    rbx + 8
    (rbx) = r12
    rbx + 8
    (rbx) = r13
    rbx + 8
    (rbx) = r14
    rbx + 8
    (rbx) = r15
    rbx - PCB_R15_OFFSET ;Reset rbx

    ;Get process's flags, rsp, and rip from stack
    rsp + 8 ;Pointer to process's flags
    r8 = (rsp) ;r8 = flags
    rsp + 8 ;Pointer to process's rsp
    r9 = (rsp) ;r9 = rsp
    rsp + 8 ;Pointer to process's rip
    r10 = (rsp) ;r10 = rip
    rsp - 24

    ;Store flags and rip
    rbx + PCB_FLAGS_OFFSET
    (rbx) = r8
    rbx - PCB_FLAGS_OFFSET
    rbx + PCB_RSP_OFFSET
    (rbx) = r9
    rbx - PCB_RSP_OFFSET
    rbx + PCB_RIP_OFFSET
    (rbx) = r10
    rbx - PCB_RIP_OFFSET ;Reset rbx

    ;Store mmu base/ limit
    r8 = cr3 ;Base
    r9 = cr4 ;Limit
    rbx + PCB_BASE_ADDR_OFFSET
    (rbx) = r8
    rbx - PCB_BASE_ADDR_OFFSET
    rbx + PCB_LIMIT_OFFSET
    (rbx) = r9
