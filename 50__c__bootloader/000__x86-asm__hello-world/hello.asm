; hello.nasm
;
; based on x86_64 nasm tutorial by 0xAX, 2014
; https://0xax.github.io/asm_1/
;
; this are my personal notes and extensions to the referred tutorial
;
;
; nasm syntax (64 bit)
;
; x86_64 basic registers
;
; rax, rbx, rcx, rdx - 64 bit basic registers
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
; typically, source is separated into .data section and .text section
;

section .data       ; define a 'data' section containing the string to print
    msg db "all work and no play makes jack a dull boy"

section .text       ; text section with entry point to the program
    global _start

_start:
    mov rax, 1      ; prepare syscall 1
    mov rdi, 1      ; prepare to pass buf    - sys_write( 'fd', ...,  ...)
    mov rsi, msg    ; prepare to print 'msg' - sys_write( ..., 'buf', ...)
    mov rdx, 42     ; size of 'msg'          - sys_write( ..., ..., count)
    syscall         ; call to function sys_write()

    mov rax, 60     ; prepare syscall 60 ie. exit call
    mov rdi, 0      ; return 0 at exit
    syscall         ; cal to funciton exit()
