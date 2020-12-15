; just some asm hello world!
;
; compile (needs nasm installed):
; $ nasm -f elf64 -o bunny.o bunny.s
; $ ld -o bunny.exe bunny.o
;
; taken from 
; https://jameshfisher.com/2018/03/10/linux-assembly-hello-world/
; author: jim fisher

global _start

section .text

_start:
  mov rax, 1        ; write(
  mov rdi, 1        ;   STDOUT_FILENO,
  mov rsi, msg      ;   "Hello, world!\n",
  mov rdx, msglen   ;   sizeof("Hello, world!\n")
  syscall           ; );

  mov rax, 60       ; exit(
  mov rdi, 0        ;   EXIT_SUCCESS
  syscall           ; );

section .rodata
  msg: db "Hello, world!", 10
  msglen: equ $ - msg


