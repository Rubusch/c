## taken from "50 lines of code" by Martin Splitt, 2018
##
## $ make
##     as -o boot.o boot.s
##     ld -o boot.bin --oformat binary -e init boot.o
##
## $ ls -l
##     total 16
##     -rwxr-xr-x 1 user user  512 Jun  5 23:31 boot.bin
##     -rw-r--r-- 1 user user 1200 Jun  5 23:31 boot.o
##     -rw-r--r-- 1 user user 1122 Jun  5 23:30 boot.s
##     -rw-r--r-- 1 user user  261 Jun  5 23:30 Makefile
##
## $ hexdump -C boot.bin
##     00000000  eb fe 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
##     00000010  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
##     *
##     000001f0  00 00 00 00 00 00 00 00  00 00 00 00 00 00 55 aa  |..............U.|
##     00000200
##
## one sector has 512 bytes - fill up to position 510 with '0', then
## append 0x55aa (twisted due to little endian)
##
## 0x55aa at the end of the first sector is the bootflag for the medium
##
## .fill takes the following arguments: how many, how often, what to put
##
## now test it in qemu
## $ qemu-system-x86_64 boot.bin

.code16                  ## set the assembler to 16bit mode
.global init

init:
        jmp init         ## just loop for the demo

.fill 510-(.-init), 1, 0 ## pad with zeros
        ## fill up with 0 to position 510, minus current position,
	## where current position '.' starts from 'init'

.word 0xaa55             ## append 0x55aa, NB: x86 is little endian!
