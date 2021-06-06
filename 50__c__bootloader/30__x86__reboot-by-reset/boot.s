## taken from "50 lines of code" by Martin Splitt, 2018
## taken parts from "kernel booting process, part 1" by 0xAX, 2014
##

##
## 'reset vector' inits a reboot
##
## the 80386 and later CPUs define the following predefined data in
## CPU registers after the computer resets
##     IP           0xfff0
##     CS selector  0xf000
##     CS base      0xffff0000
##
## the processor starts working in 'real mode'
##
##
## addressing
##
## an address consists of two parts: a segment selector, which has a base address
## and an offset from this base address, in real mode, the associated base address
## of a segment selector is 'segment selector' * 16 i.e.
## physical address = segment selector * 16 + offset
## i.e.
## physical address = (segment selector << 4) + offset
##
## for example, if CS:IP is 0x2000:0x0010, then the corresponding physical address
## will be 0x20010
##
## in python, try
## >>> hex((0x2000 << 4) + 0x0010)
##
## now test it in qemu
## $ qemu-system-x86_64 boot.bin
.code16
.global init

init:
        ljmpw $0xffff, $0    ## jump to 'reset vector', i.e. reboot

.fill 510-(.-init), 1, 0
.word 0xaa55
