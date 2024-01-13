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