; hello.nasm
;
; based on x86_64 nasm tutorial by 0xAX, 2014 (twitter.com/0xAX)
; https://0xax.github.io/asm_5/
;
; based on nasm manual
; https://nasm.us/xdoc/2.15rc9/html/nasmdoc5.html
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
; function
;
; function: call arguments
;   rdi - first argument
;   rsi - second argument
;   rdx - third argument
;   rcx - fourth argument
;   r8  - fifth argument
;   r9  - sixth argument
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
; instructions
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
; instructions: push
;    pusha - push all registers
;    pushf - push all flags
;    popa  - pop back all register contents
;    popf  - pop back all flags
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
;   rep   - repeat while rcx is not zero
;   lodsb - read byte to string
;   movsb - copy a string of bytes (movsw, movsd and etc..)
;   cmpsb - byte string comparison
;   scasb - byte string scanning
;   stosb - write byte to string
;
;
; macros
;
; macros: single-line
;     %define macro_name(parameter) value
;
; example
;     %define argc rsp + 8
;
; usage
;     mov rax, [argc]
;     cmp rax, 3
;     jne .mustBe3args
;
;
; macros: multiline
; starts with %macro nasm directive and end with %endmacro
;     %macro number_of_parameters
;         instruction
;         instruction
;         instruction
;     %endmacro
;
; example
;     %macro bootstrap 1
;               push ebp
;               mov ebp,esp
;     %endmacro
;
; usage
;     _start:
;         bootstrap
;
; macros: notes
; - all labels which defined in macro must start with %%
; - STRUC macros, use c-like struct in assembler
;
;
; push/pop
;
; push and pop in x86_64bit can be tricky, see
; https://www.cs.uaf.edu/2015/fall/cs301/lecture/09_16_stack.html
;
;

        ;; this is just a definition w/o allocation of a struct 'agenda'
struc agenda
    .name   resb 10
    .number resb 10
    .type   resb 10
endstruc


        ;; some global constants
section .data
    SYS_EXIT equ 60
    EXIT_SUCCESS equ 0

        ;; the struct 'agenda' is allocated in .bss with variable / un-initialized sizes
section .bss
    myagenda: resb agenda_size

section .text
    global _start

        ;; usage of agenda's instance 'myagenda' and field 'agenda.name' is here
_start:
    mov eax, myagenda + agenda.name

    jmp exit                    ; just exit the proggy

exit:
    mov rax, SYS_EXIT
    mov rdi, EXIT_SUCCESS
    syscall

        ;; NOTE
        ;; - issue: error: identifier expected after global, got `: _start'
        ;;      there was a ':' after 'global' - removed it!
