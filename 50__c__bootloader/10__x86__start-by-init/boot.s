## taken from "50 lines of code" by Martin Splitt, 2018
##
## $ make
##     as -o boot.o boot.s
##     ld -o boot.bin --oformat binary -e init boot.o
##
## provide format: raw (binary), no ELF overhead
## set entry point: 'init'
##
## $ ls -l
##     total 16
##     -rwxr-xr-x 1 user user   2 Jun  5 23:17 boot.bin
##     -rw-r--r-- 1 user user 696 Jun  5 23:17 boot.o
##     -rw-r--r-- 1 user user 531 Jun  5 23:06 boot.s
##     -rw-r--r-- 1 user user 339 Jun  5 23:16 Makefile
##
## $ hexdump -C boot.bin
##     00000000  eb fe                                             |..|
##     00000002
##
## the size of 2 byte for boot.bin is now much better, but to fill up
## the first sector we need exactly 512 byte, where the last two
## byte need to be 0x55 and 0xaa

.code16           ## set the assembler to 16bit mode
.global init      ## set the _start point

init:
        jmp init  ## just loop for the demo
