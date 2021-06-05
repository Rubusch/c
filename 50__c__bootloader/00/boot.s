## taken from "50 lines of code" by Martin Splitt, 2018
##
## $ make
##     as -o boot.o boot.s
## $ ls -l
##     total 12
##     -rw-r--r-- 1 user user 696 Jun  5 23:04 boot.o
##     -rw-r--r-- 1 user user 203 Jun  5 23:04 boot.s
##     -rw-r--r-- 1 user user 198 Jun  5 23:00 Makefile
##
## as described boot.o will be already aroudn 696 bytes, due to object
## code and ELF format

.code16           ## set the assembler to 16bit mode
.global init      ## set the _start point

init:
        jmp init  ## just loop for the demo
