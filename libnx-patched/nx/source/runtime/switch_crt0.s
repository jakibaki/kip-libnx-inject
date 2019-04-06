.macro push_all
    sub sp, sp, #0x100
    stp x29, x30, [sp, #0x0]
    stp x27, x28, [sp, #0x10]
    stp x25, x26, [sp, #0x20]
    stp x23, x24, [sp, #0x30]
    stp x21, x22, [sp, #0x40]
    stp x19, x20, [sp, #0x50]
    stp x17, x18, [sp, #0x60]
    stp x15, x16, [sp, #0x70]
    stp x13, x14, [sp, #0x80]
    stp x11, x12, [sp, #0x90]
    stp x9, x10, [sp, #0xA0]
    stp x7, x8, [sp, #0xB0]
    stp x5, x6, [sp, #0xC0]
    stp x3, x4, [sp, #0xD0]
    stp x1, x2, [sp, #0xE0]
    str x0, [sp, #0xF0]
.endm

.macro pop_all
    ldr x0, [sp, #0xF0]
    ldp x1, x2, [sp, #0xE0]
    ldp x3, x4, [sp, #0xD0]
    ldp x5, x6, [sp, #0xC0]
    ldp x7, x8, [sp, #0xB0]
    ldp x9, x10, [sp, #0xA0]
    ldp x11, x12, [sp, #0x90]
    ldp x13, x14, [sp, #0x80]
    ldp x15, x16, [sp, #0x70]
    ldp x17, x18, [sp, #0x60]
    ldp x19, x20, [sp, #0x50]
    ldp x21, x22, [sp, #0x40]
    ldp x23, x24, [sp, #0x30]
    ldp x25, x26, [sp, #0x20]
    ldp x27, x28, [sp, #0x10]
    ldp x29, x30, [sp, #0x0]
    add sp, sp, #0x100
.endm


.section ".crt0","ax"
.global _start

_start:
    b startup
    .word __nx_mod0 - _start
// Start handlers for hooked functions:
.org _start+0xc
    b hooked_sdmmc_execute_cmd

.org _start+0x80
startup:
    // save lr
    mov  x7, x30

    // get aslr base
    bl   +4
    sub  x6, x30, #0x88


    // context ptr and main thread handle
    mov  x5, x0
    mov  x4, x1

    // Handle the exception if needed.
    // if (ctx != NULL && main_thread != -1)__libnx_exception_entry(<inargs>);
    cmp x5, #0
    ccmn x4, #1, #4, ne // 4 = Z
    beq bssclr_start
    b __libnx_exception_entry

bssclr_start:

    // START MY CODE
    push_all

    // Since we're injecting a .text section we need to remap our .data and .bss sections as rw which we do here
    mov w0, #0xFFFF8001
    adr x1, __data_start__
    adr x2, __argdata__
    sub x2, x2, x1
    mov x3, #3
    svc 0x73

    pop_all
    // END MY CODE

    mov x27, x7
    mov x25, x5
    mov x26, x4

    // clear .bss
    adrp x0, __bss_start__
    adrp x1, __bss_end__
    add  x0, x0, #:lo12:__bss_start__
    add  x1, x1, #:lo12:__bss_end__
    sub  x1, x1, x0  // calculate size
    add  x1, x1, #7  // round up to 8
    bic  x1, x1, #7

bss_loop: 
    str  xzr, [x0], #8
    subs x1, x1, #8
    bne  bss_loop

    // store stack pointer
    mov  x1, sp
    adrp x0, __stack_top
    str  x1, [x0, #:lo12:__stack_top]

    // process .dynamic section
    mov  x0, x6
    adrp x1, _DYNAMIC
    add  x1, x1, #:lo12:_DYNAMIC
    bl   __nx_dynamic


    // initialize system
    mov  x0, x25
    mov  x1, x26
    mov  x2, x27
    bl   __libnx_init

    // START MY CODE
    bl __injected_init 

    // Set stuff back to how nintendo wants it
    mov x0, x25
    mov x1, x26
    mov x30, x27
    b _start + 0x100000

    // call entrypoint
    /*adrp x0, __system_argc // argc
    ldr  w0, [x0, #:lo12:__system_argc]
    adrp x1, __system_argv // argv
    ldr  x1, [x1, #:lo12:__system_argv]
    adrp x30, exit
    add  x30, x30, #:lo12:exit
    b    main*/

    // The infinite loop of shame
failed_loop:
    b failed_loop

    // END MY CODE

.global __nx_exit
.type   __nx_exit, %function
__nx_exit:
    // restore stack pointer
    adrp x8, __stack_top
    ldr  x8, [x8, #:lo12:__stack_top]
    mov  sp, x8

    // jump back to loader
    br   x1

.global __nx_mod0
__nx_mod0:
    .ascii "MOD0"
    .word  _DYNAMIC             - __nx_mod0
    .word  __bss_start__        - __nx_mod0
    .word  __bss_end__          - __nx_mod0
    .word  __eh_frame_hdr_start - __nx_mod0
    .word  __eh_frame_hdr_end   - __nx_mod0
    .word  0 // "offset to runtime-generated module object" (??)
