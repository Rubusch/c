## taken from "50 lines of code" by Martin Splitt, 2018
## taken parts from "kernel booting process, part 1" by 0xAX, 2014
##
##
## GNU Assembler
##
##
## x86 asm: basics (32 bit)
##
## general purpose registers: ax, bx, cx, dx - each -h and -l, e.g. ah+al
## source index:              si
## destination index:         di
## stack pointer:             sp
## base pointer:              bp
##
##
## x68 asm: basic instructions
##
## mov : moves data between two locations
## cmp : compares two things e.g. register values
## jmp : jump to another position in the code
## je  : jumps if the things in the previous 'cmp' were equal [0]
## inc : increments the value in the given register or memory address
## dec : decrements the value in the given register or memory address
## int : call an interrupt
## lods : string helper instruction to load a character
## hlt : halt signal
##
##
## instructions can be suffixed
##
## -b  : byte,   8 bits
## -w  : word,  16 bits
## -s  : short, 16 bits
## -l  : long,  32 bits (int)
## -q  : quad,  64 bits
##
##
## arguments can be prefixed
##
## %...   : references a REGISTER
## $...   : means a VALUE
## (%...) : means an ADDRESS stored as value in a register, e.g. (%bx)
##
##
## usually the GNU assembler is quite good at figuring out sizes
## without the suffixes, but sometimes it can't guess, e.g.
##
##     mov $0x123, %bx    # loads value 0x123 into register bx
##     movb %5, (%bx)     # loads a single byte, value 5, into memory at address 0x123
##
##
## print a single character
##
## function teletype - we can select that function by setting ah to
## 0xe, put the ASCII code we want to print into al and then call
##     'int 0x10' (code16)
##
.code16
.global init

init:
        mov $0x0e41, %ax    ## sets AH to 0x0e (function teletype) and AL to 0x41 (ASCII "A")
        int $0x10           ## call to interrupt 0x10
        hlt                 ## stop execution

.fill 510-(.-init), 1, 0
.word 0xaa55
