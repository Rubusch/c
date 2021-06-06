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
## $...   : means a VALUE, or ADDRESS in case of a string
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
## x86 asm: labels
##
## preprocessor directive | bytes reserved | meaning
## ----------------------------------------------------------------
##     .byte              |       1        | reserves  8 bits
##     .word              |       2        | reserves 16 bits
##     .short             |       2        | alias for .word
##     .long              |       4        | reserves 32 bits
##     .int               |       4        | alias for .long
##     .quad              |       8        | reserves 64 bits
##     .ascii             |       *        | reserves as many bytes as needed for a string
##     .asciz             |       *        | as .ascii, 0 terminated (additional)
##
##
## print a string, part 2
##
## for string printing, we will load the address of 'msg' into the
## special register SI which allows us to use the convenient lodsb
## instructions that loads a byte from the address that SI points to
## into AL and increments the address in SI at the same time
##
## setup a zero terminated ASCII label, and loop over printing each
## character individually
##
.code16
.global init

init:
        mov $0x0e, %ah      ## load 0x0e (function teletype) into ah
        mov $msg, %si       ## load address of 'msg' into si
print_char:
        lodsb               ## loads a byte from the address in si into al and increments si
        cmp $0, %al         ## already at the end?
        je print_done       ##     -> yes, jump out
        int $0x10           ##     -> else, print via interrupt 0x10
        jmp print_char      ## loop
print_done:
        hlt                 ## done

msg: .asciz "All work and no play makes Jack a dull boy!"

.fill 510-(.-init), 1, 0
.word 0xaa55
