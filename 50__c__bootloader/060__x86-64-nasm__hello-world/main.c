/*
 get assembly by gcc:
 $ gcc -S ./main.c



 nasm and gas (Gnu ASembler)

 references:
 https://0xax.github.io/asm_6/

 gas: data directives

        // 1 byte
        var1: .byte 10

        // 2 byte
        var2: .word 10

        // 4 byte
        var3: .int 10

        // 8 byte
        var4: .quad 10

        // 16 byte
        var5: .octa 10

        // assembles each string (with no automatic trailing zero byte) into consecutive addresses
        str1: .asci "Hello world"

        // just like .ascii, but each string is followed by a zero byte
        str2: .asciz "Hello world"

        // Copy the characters in str to the object file
        str3: .string "Hello world"


 gas: back order

        mov source, destination

        NB:
        registers start with %
        direct operands start with $


gas: specify size

        mov ax, word [rsi]


gas: specify size in instruction

        movw (%rsi), %ax


gas: postfixes for instructions

        b - 1 byte operands
        w - 2 bytes operands
        l - 4 bytes operands
        q - 8 bytes operands
        t - 10 bytes operands
        o - 16 bytes operands

gas: far function call and jumps

        lcall $section, $offset



 ---
 Q&A:
        ...
	movl	$15, %edx
	leaq	.LC0(%rip), %rsi
	movl	$1, %edi
	call	write@PLT
        ...

 -> loads 3rd, 2nd and 1st argument, then calls write()


 why leaq for loading the string from lable into rsi?

 lea - load effective address is used to load arrays more
 conveniently, than a series of movs



 why the function call using PIC?

 also try
 $ gcc -S -fPIC -fPIE ./main.c


 see:
 https://stackoverflow.com/questions/36968287/why-doesnt-gcc-reference-the-plt-for-function-calls

 When compiling a position-independent executable (with -fPIE), the
 linked binary jumps to puts through the PLT, identically to without
 -fPIC. However, the compiler asm output is different (try it yourself
 on the godbolt link above):

 call_puts:  # compiled with -fPIE
    leaq    .LC0(%rip), %rdi      # RIP-relative addressing for static data
    jmp     puts@PLT

 The differences between -fPIC and -fPIE are mainly that PIE can
 assume no symbol interposition for functions in the same compilation
 unit, but PIC can't. OS X requires position-independent executables,
 as well as shared libraries, but there is a difference in what the
 compiler can do when making code for a library vs. making code for an
 executable.

 analyze with objdump
 $ objdump -dr ./main.s

*/
#include <unistd.h>

int main(void)
{
	write(1, "Hello World\n", 15);
	return 0;
}
