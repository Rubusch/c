; hello.nasm
;
; based on x86_64 nasm tutorial by 0xAX, 2014 (twitter.com/0xAX)
; https://0xax.github.io/asm_4/
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
; function: call arguments
;
; rdi - first argument
; rsi - second argument
; rdx - third argument
; rcx - fourth argument
; r8  - fifth argument
; r9  - sixth argument
;
;
; function: return
;
; we pushed symbols from our string to stack and now there is no
; return address of stack top and function doesn’t know where to
; return
;
; note:
;    $ - returns position in memory of string where $ defined
;   $$ - returns position in memory of current section start
;
; function return - example:
;     mov rdi, $ + 15
;
; why 15?
; $ objdump -D hello.bin
;     ...
;     Disassembly of section .text:
;
;     0000000000401000 <_start>:
;       401000:       48 be 01 20 40 00 00    movabs $0x402001,%rsi
;       401007:       00 00 00
;       40100a:       48 31 c9                xor    %rcx,%rcx
;       40100d:       fc                      cld
;       40100e:       48 bf 1d 10 40 00 00    movabs $0x40101d,%rdi
;       401015:       00 00 00
;       401018:       e8 08 00 00 00          callq  401025 <str_length>
;       40101d:       48 31 c0                xor    %rax,%rax
;       401020:       48 31 ff                xor    %rdi,%rdi
;       401023:       eb 0e                   jmp    401033 <reverse_str>
;     ...
;
; => moving stuff to rdi via 'movabs' takes 10 bytes:
;         48 bf 1d 10 40 00 00    movabs $0x40101d,%rdi
;         00 00 00
;    plus another 5 bytes for the immediately followed 'callq'
;         e8 08 00 00 00          callq  401025 <str_length>
;    results in 15 bytes total
;
; since we want to jump back from the call ('callq') and need to know
; from where we moved stuff to rsi ('movabs'), we need the position:
; $ - 15
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
;     ja  - the same that jg, but performs unsigned comparison
;     jae - the same that jge, but performs unsigned comparison
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
;
;
; instructions for string/bytes manipulations:
;
;   rep   - repeat while rcx is not zero
;   lodsb - read byte to string
;   movsb - copy a string of bytes (movsw, movsd and etc..)
;   cmpsb - byte string comparison
;   scasb - byte string scanning
;   stosb - write byte to string
;
;


section .data
    SYS_WRITE equ 1
    STD_OUT equ 1
    SYS_EXIT equ 60
    EXIT_SUCCESS equ 0

    NEW_LINE db 0xa
    INPUT db "...murder murder murder"

section .bss            ; bss section - global, uninitialized data
    OUTPUT resb 12

section .text
    global _start


_start:
    mov rsi, INPUT
    xor rcx, rcx        ; memzero rcx
    cld                 ; set df flag to zero, needed for string parsing
    mov rdi, $ + 15     ; remember place after function call - see explanation in text
    call str_length
    xor rax, rax
    xor rdi, rdi
    jmp reverse_str


;; just calculates length of INPUT string and store result in rcx
;; register - as we executed cld instruction, lodsb will everytime
;; move rsi to one byte from left to right, so we will move by string
;; symbols
;; after, we push the rax value to the stack, now it contains all
;; symbols of our string
str_length:
    cmp byte [rsi], 0   ; jump out, if string was empty
    je exit_from_routine
    lodsb               ; load byte from rsi to al and inc rsi
    push rax            ; push symbol to stack
    inc rcx             ; increase counter
    jmp str_length      ; loop


exit_from_routine:
    push rdi            ; fetch stored position to jump "back" to _start - see explanation in text
    ret


;; get the reversed string in OUTPUT buffer and write the result to stdout, and append a new line
reverse_str:
    cmp rcx, 0
    je print            ; print, once rcx (our counter of string) is "done"
    pop rax             ; pop character from stack into rax, then print it in OUTPUT buffer
    mov [OUTPUT + rdi], rax ; add rdi, otherwise we’ll write the character only into the first byte of buffer
    dec rcx             ; decrement the length of the string characters on the stack
    inc rdi             ; increment rdi to move to the next element
    jmp reverse_str


print:
    mov rdx, rdi
    mov rax, 1
    mov rdi, 1
    mov rsi, OUTPUT
    syscall
    jmp print_newline


print_newline:
    mov rax, SYS_WRITE
    mov rdi, STD_OUT
    mov rsi, NEW_LINE
    mov rdx, 1
    syscall
    jmp exit


exit:
    mov rax, SYS_EXIT
    mov rdi, EXIT_SUCCESS
    syscall

