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