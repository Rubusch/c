; hello.nasm
;
; based on x86_64 nasm tutorial by 0xAX, 2014 (twitter.com/0xAX)
; https://0xax.github.io/asm_3/
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
; function call
;
; rdi - first argument
; rsi - second argument
; rdx - third argument
; rcx - fourth argument
; r8  - fifth argument
; r9  - sixth argument
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
; effective address - e.g. to move just one byte of a register /
;     compare content of one register, use []
;
;
; sections of an asm program
;    data - section is used for declaring initialized data or constants
;    bss  - section is used for declaring non initialized (global) variables
;    text - section is used for code
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
;
; stack
;
; Stack works as LIFO (Last In First Out)
;
; push argument - increments stack pointer (RSP) and stores argument
;                 in location pointed by stack pointer
; pop argument  - copied data to argument from location pointed by
;                 stack pointer
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
    SYS_WRITE equ 1
    STD_IN equ 1
    SYS_EXIT equ 60
    EXIT_SUCCESS equ 0

    NEW_LINE db 0xa
    WRONG_ARGC db "must be two command line args", 0xa

section .text
    global _start

_start:
    pop rcx                    ; pop from stack into rcx
    cmp rcx, 3                 ; check 3 arguments, i.e. program name, first arg, second argument
    jne argc_error

    add rsp, 8                 ; shift the stack pointer by 8, to fetch first argument, where
                               ;    [rsp] - top of stack will contain arguments count.
                               ;    [rsp + 8] - will contain argv[0]
                               ;    [rsp + 16] - will contain argv[1]
                               ;    and so on...
    pop rsi                    ; then, pop the stack value pointed to by rsp
    call str_to_int

    mov r10, rax
    pop rsi
    call str_to_int
    mov r11, rax

    add r10, r11               ; sum it
    mov rax, r10               ; prepare call to convert to string to print it
    xor r12, r12               ; memzero - set r12 to zero
    jmp int_to_str

argc_error:
    mov rax, 1                 ; prepare syscall WRITE
    mov rdi, 1
    mov rsi, WRONG_ARGC
    mov rdx, 30
    syscall
    jmp exit

;; convert string to integer
;; expects argv[1] in rsi of stack
str_to_int:
    xor rax, rax               ; init rax to 0 - memzero rax
    mov rcx, 10                ; init rcx to 10
next:
    cmp [rsi], byte 0          ; compare the first byte of rsi with '0'
    je return_str
    mov bl, [rsi]              ; if it was not '0', move one byte from it to bl
    sub bl, 48                 ; and subtract 48 from it - Why 48? All numbers from 0 to 9 have 48 to 57 codes in asci table
    mul rcx                    ; then multiply by 10s (rcx was inited to 10) for decimal position
    add rax, rbx               ; and add it
    inc rsi
    jmp next
return_str:
    ret


int_to_str:
    mov rdx, 0                 ; init rdx and rbx
    mov rbx, 10
    div rbx                    ; NB: div puts remainder in rdx!!!
    add rdx, 48
    add rdx, 0x0               ; append \0 to end the string
    push rdx
    inc r12
    cmp rax, 0x0               ; compare if rax already 0 (i.e. end of string conversion)
    jne int_to_str
    jmp print                  ; print result

print:
    ;; compute string length
    mov rax, 1
    mul r12
    mov r12, 8
    mul r12                    ; multiply by 8, to shift position on the stack
    mov rdx, rax

    ;; print sum
    mov rax, SYS_WRITE
    mov rdi, STD_IN
    mov rsi, rsp
    syscall
    jmp print_new_line

print_new_line:
    mov rax, SYS_WRITE
    mov rdi, STD_IN
    mov rsi, NEW_LINE
    mov rdx, 1
    syscall
    jmp exit

exit:
    mov rax, SYS_EXIT
    mov rdi, EXIT_SUCCESS
    syscall
