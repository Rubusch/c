; hello.nasm
;
; based on x86_64 nasm tutorial by 0xAX, 2014
; https://0xax.github.io/asm_2/
;
; based on nasm manual,
; https://nasm.us/doc/nasmdoc3.html
;
;
; this are my personal notes and extensions to the referred tutorial
;
;
;
; nasm syntax (64 bit)
;
; x86_64 basic registers
;
; rax, rbx, rcx, rdx, rbp, rsp, rsi, rdi, r8, r9, r10, r11, r12, r13, r14, r15 - 64 bit general purpose registers
;                rax - when we call a syscal, rax must contain syscall number
;                rdx - used to pass 3rd argument to functions
;
; rsi                - 64 bit source index, used to pass 2nd argument to functions
; rdi                - 64 bit destination index, used to pass 1st argument to functions
; rsp                - 64 bit stack pointer
; rbp                - 64 bit base pointer
; rip                - 64 bit instruction pointer
;
;
; so, for function arguments, use: rdi, rsi, rdx
; e.g.
;     size_t sys_write(unsigned int fd, const char * buf, size_t count);
;     -> fd    - rdi
;     -> *buf  - rsi
;     -> count - rdx
;
;
; every nasm instruction has the form
;     [label:] instruction [operands] [; comment]
;
; e.g.
;     mov count, 48           ; put value 48 into count
;
; remember as
;     what?  to? , from where? [W.T.F.]
;
;
; sections of an asm program
;    data - section is used for declaring initialized data or constants
;    bss  - section is used for declaring non initialized (global) variables
;    text - section is used for code
;
;
;
; data
;
; variables are declared in section .data, example:
;
;     section .data
;         num1:   equ 100
;         num2:   equ 50
;         msg:    db "Sum is correct", 10
;
; delcare two constants num1 and num2 and an initialized string msg,
; append 10 to the string
;
; data: initialized data
;     db - declare byte, 8 bits
;     dw - declare word, 16 bits
;     dd - declare double word, 32 bits
;     dq - declare quad word, 64 bits
;     dt - float, extended precision
;     do - float, IEEE 754r quad precision
;     dy - float, TODO  
;     dz - float, TODO  
;
; data: non-initialized data
;     resb - declare non-initialized byte, 8 bits
;     resw - declare non-initialized word, 16 bits
;     resd - declare non-initialized double word, 32 bits
;     resq - declare non-initialized quad word, 64 bits
;     rest - float, non-initialized, extended precision
;     reso - float, non-initialized, TODO  
;     resy - float, non-initialized, TODO  
;     resz - float, non-initialized, TODO  
;
; data: misc
;     incbin - external binary files
;     equ    - defines an initialized constant
;     times  - repeating instructions or data
;
;
; x86_64 basic commands
;
; arithmetics
;     add  - integer add
;     sub  - substract
;     mul  - unsigned multiply
;     imul - signed multiply
;     div  - unsigned divide
;     idiv - signed divide
;     inc  - increment
;     dec  - decrement
;     neg  - negate
;
; x86_64 constrol flow
;
;     je - jump if equal
;     jz - if zero
;     jne - if not equal
;     jnz - if not zero
;     jg  - if first operand is greater than second
;     jge - if first operand is greater or equal to second
;     ja  - the same that JG, but performs unsigned comparison
;     jae - the same that JGE, but performs unsigned comparison
;
; example:
;     if (rax != 50) {
;         exit();
;     } else {
;         right();
;     }
;
; will become
;
;     ;; compare rax with 50
;     cmp rax, 50
;     ;; perform .exit if rax is not equal 50
;     jne .exit
;     jmp .right
;
;
; syscalls
;
; find syscalls in table
; https://github.com/torvalds/linux/blob/master/arch/x86/entry/syscalls/syscall_64.tbl
;
; used here are
;     1  sys_write
;    60  exit

section .data
    num1:   equ 100                   ; define constants, and message
    num2:   equ 50
    msg:    db "all work and no play makes jack a dull boy"

section .text

    global _start

; entry point
_start:
    mov rax, num1                     ; set num1's value to rax
    mov rbx, num2                     ; set num2's value to rbx
    add rax, rbx                      ; get sum of rax and rbx, and store it's value in rax

    cmp rax, 150                      ; compare rax with 150
    jne .exit                         ; jump to '.exit' label if rax and 150 are not equal

    jmp .right_sum                    ; else, jump to '.rightSum' if rax and 150 are equal

.right_sum:                           ; print message that sum is correct via sys_write(), syscall 1
    mov     rax, 1                    ; syscall number
    mov     rdi, 1                    ;     arg1: fd = 1
    mov     rsi, msg                  ;     arg2: buf = msg
    mov     rdx, 42                   ;     arg3: count = 42, strlen(msg)
    syscall

    jmp .exit                         ; end program

; exit procedure
.exit:                                ; prepare exit(0), syscall 60
    mov    rax, 60
    mov    rdi, 0
    syscall
