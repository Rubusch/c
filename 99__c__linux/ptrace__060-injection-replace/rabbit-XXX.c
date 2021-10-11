/*
  Hello World

  after sleeping for 3 sec the rabbit.exe is going to enter its asm instructions
  it prints Hello World and falls into trap

  The example is a modernized version of P. Padala's jump-example.

  The jumping backward and forward here is required to find the address of the
  "hello world" string. (P. Padala, Linux Journal, 2003)

  ---

  GAS (GCC Assembler Syntax)

  GCC, the GNU C Compiler for Linux, uses AT&T/UNIX assembly syntax. Here we’ll
  be using AT&T syntax for assembly coding. Don’t worry if you are not familiar
  with AT&T syntax, I will teach you. This is quite different from Intel
  syntax. I shall give the major differences.


  1. Source-Destination Ordering.

  The direction of the operands in AT&T syntax is opposite to that of Intel. In
  Intel syntax the first operand is the destination, and the second operand is
  the source whereas in AT&T syntax the first operand is the source and the
  second operand is the destination. ie, "Op-code dst src" in Intel syntax
  changes to "Op-code src dst" in AT&T syntax.


  2. Register Naming.

  Register names are prefixed by % ie, if eax is to be used, write %eax.


  3. Immediate Operand.

  AT&T immediate operands are preceded by ’$’. For static "C" variables also
  prefix a ’$’. In Intel syntax, for hexadecimal constants an ’h’ is suffixed,
  instead of that, here we prefix ’0x’ to the constant. So, for hexadecimals,
  we first see a ’$’, then ’0x’ and finally the constants.


  4. Operand Size.

  In AT&T syntax the size of memory operands is determined from the last
  character of the op-code name. Op-code suffixes of ’b’, ’w’, and ’l’ specify
  byte(8-bit), word(16-bit), and long(32-bit) memory references. Intel syntax
  accomplishes this by prefixing memory operands (not the op-codes) with
  ’byte ptr’, ’word ptr’, and ’dword ptr’.

  Thus, Intel "mov al, byte ptr foo" is "movb foo, %al" in AT&T syntax.


  5. Memory Operands.

  In Intel syntax the base register is enclosed in ’[’ and ’]’ where as in AT&T
  they change to ’(’ and ’)’. Additionally, in Intel syntax an indirect memory
  reference is like

  section:[base + index*scale + disp], which changes to

  section:disp(base, index, scale) in AT&T.

  One point to bear in mind is that, when a constant is used for disp/scale, ’$’
  shouldn’t be prefixed.

  Now we saw some of the major differences between Intel syntax and AT&T syntax.
  I’ve wrote only a few of them. For a complete information, refer to GNU
  Assembler documentations. Now we’ll look at some examples for better
  understanding.

    +------------------------------+------------------------------------+
    |       Intel Code             |      AT&T Code                     |
    +------------------------------+------------------------------------+
    | mov     eax,1                |  movl    $1,%eax                   |
    | mov     ebx,0ffh             |  movl    $0xff,%ebx                |
    | int     80h                  |  int     $0x80                     |
    | mov     ebx, eax             |  movl    %eax, %ebx                |
    | mov     eax,[ecx]            |  movl    (%ecx),%eax               |
    | mov     eax,[ebx+3]          |  movl    3(%ebx),%eax              |
    | mov     eax,[ebx+20h]        |  movl    0x20(%ebx),%eax           |
    | add     eax,[ebx+ecx*2h]     |  addl    (%ebx,%ecx,0x2),%eax      |
    | lea     eax,[ebx+ecx]        |  leal    (%ebx,%ecx),%eax          |
    | sub     eax,[ebx+ecx*4h-20h] |  subl    -0x20(%ebx,%ecx,0x4),%eax |
    +------------------------------+------------------------------------+

  NOTE: for amt64, you need to use 'syscall' instead of 'int 0x80'

  NOTE: a better version was for nowadays to use 'explicit variables'

  NOTE: In basic inline assembly, we had only instructions. In extended
  assembly, we can also specify the operands. It allows us to specify the
  input registers, output registers and a list of clobbered registers. It
  is not mandatory to specify the registers to use, we can leave that head
  ache to GCC and that probably fit into GCC’s optimization scheme better.
  Anyway the basic format is:

           asm ( assembler template
               : output operands                  / * optional * /
               : input operands                   / * optional * /
               : list of clobbered registers      / * optional * /
               );

  NOTE: To specify the register, you must directly specify the register names by using specific register constraints. They are:

    +---+--------------------+
    | r |    Register(s)     |
    +---+--------------------+
    | a |   %eax, %ax, %al   |
    | b |   %ebx, %bx, %bl   |
    | c |   %ecx, %cx, %cl   |
    | d |   %edx, %dx, %dl   |
    | S |   %esi, %si        |
    | D |   %edi, %di        |
    +---+--------------------+

  NOTE: rabbit.exe was stopped, w/o being a child, nor having declared
  PTRACE_TRACEME explicitely!

  ---

  disassembling

  how to obtain the "hello world" instructions with gdb?

  compile it run it in gdb and extract the ASM sequence for the trap as hex

  compile:
  $ gcc -g -Wall -fPIC -no-pie -std=gnu99 -pedantic -o rabbit.exe rabbit.c
  or run
  $ make

  execute then in gdb
  $ gdb ./rabbit.exe
    GNU gdb (Debian 8.2.1-2+b3) 8.2.1
    Copyright (C) 2018 Free Software Foundation, Inc.
    License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
    This is free software: you are free to change and redistribute it.
    There is NO WARRANTY, to the extent permitted by law.
    Type "show copying" and "show warranty" for details.
    This GDB was configured as "x86_64-linux-gnu".
    Type "show configuration" for configuration details.
    For bug reporting instructions, please see:
    <http://www.gnu.org/software/gdb/bugs/>.
    Find the GDB manual and other documentation resources online at:
        <http://www.gnu.org/software/gdb/documentation/>.

    For help, type "help".
    Type "apropos word" to search for commands related to "word"...
    Reading symbols from ./rabbit.exe...done.

  (gdb) disassemble main
    Dump of assembler code for function main:
       0x0000000000401132 <+0>:     push   %rbp
       0x0000000000401133 <+1>:     mov    %rsp,%rbp
       0x0000000000401136 <+4>:     mov    $0x3,%edi
       0x000000000040113b <+9>:     callq  0x401040 <sleep@plt>
       0x0000000000401140 <+14>:    jmp    0x40115b <main+41>
       0x0000000000401142 <+16>:    pop    %rsi
       0x0000000000401143 <+17>:    mov    $0x1,%rax
       0x000000000040114a <+24>:    mov    $0x2,%rdi
       0x0000000000401151 <+31>:    mov    $0x12,%rdx
       0x0000000000401158 <+38>:    syscall
       0x000000000040115a <+40>:    int3
       0x000000000040115b <+41>:    callq  0x401142 <main+16>
       0x0000000000401160 <+46>:    rex.W
       0x0000000000401161 <+47>:    gs insb (%dx),%es:(%rdi)
       0x0000000000401163 <+49>:    insb   (%dx),%es:(%rdi)
       0x0000000000401164 <+50>:    outsl  %ds:(%rsi),(%dx)
       0x0000000000401165 <+51>:    and    %bl,0x58(%rax)
       0x0000000000401168 <+54>:    pop    %rax
       0x0000000000401169 <+55>:    and    %dl,0x6f(%rdi)
       0x000000000040116c <+58>:    jb     0x4011da <__libc_csu_init+90>
       0x000000000040116e <+60>:    or     %fs:(%rax),%al
       0x0000000000401171 <+63>:    add    %bh,0x0(%rdi)
       0x0000000000401177 <+69>:    callq  0x401030 <exit@plt>
    End of assembler dump.

  we need to take the machine code bytes from main+3 to backward+20, which is a
  total of around 41 bytes; obtain this sequence as HEX

  (gdb) help x
    Examine memory: x/FMT ADDRESS.
    ADDRESS is an expression for the memory address to examine.
    FMT is a repeat count followed by a format letter and a size letter.
    Format letters are o(octal), x(hex), d(decimal), u(unsigned decimal),
      t(binary), f(float), a(address), i(instruction), c(char), s(string)
      and z(hex, zero padded on the left).
    Size letters are b(byte), h(halfword), w(word), g(giant, 8 bytes).
    The specified number of objects of the specified size are printed
    according to the format.  If a negative number is specified, memory is
    examined backward from the address.

    Defaults for format and size letters are those previously used.
    Default count is 1.  Default address is following last thing printed
    with this command or "print".

  (gdb) x/80bx main
    0x401132 <main>:        0x55    0x48    0x89    0xe5    0xbf    0x03    0x00    0x00
    0x40113a <main+8>:      0x00    0xe8    0x00    0xff    0xff    0xff    0xeb    0x19
    0x401142 <main+16>:     0x5e    0x48    0xc7    0xc0    0x01    0x00    0x00    0x00
    0x40114a <main+24>:     0x48    0xc7    0xc7    0x02    0x00    0x00    0x00    0x48
    0x401152 <main+32>:     0xc7    0xc2    0x12    0x00    0x00    0x00    0x0f    0x05
    0x40115a <main+40>:     0xcc    0xe8    0xe2    0xff    0xff    0xff    0x48    0x65
    0x401162 <main+48>:     0x6c    0x6c    0x6f    0x20    0x58    0x58    0x58    0x20
    0x40116a <main+56>:     0x57    0x6f    0x72    0x6c    0x64    0x0a    0x00    0x00
    0x401172 <main+64>:     0xbf    0x00    0x00    0x00    0x00    0xe8    0xb4    0xfe
    0x40117a <main+72>:     0xff    0xff    0x0f    0x1f    0x40    0x00    0x41    0x57

  cut out sequence starting with "0xeb 0x19 0x5e", and ending with "0x64 0x0a 0x00"

  (gdb) x/49bx main+14
    0x401140 <main+14>:     0xeb    0x19    0x5e    0x48    0xc7    0xc0    0x01    0x00
    0x401148 <main+22>:     0x00    0x00    0x48    0xc7    0xc7    0x02    0x00    0x00
    0x401150 <main+30>:     0x00    0x48    0xc7    0xc2    0x12    0x00    0x00    0x00
    0x401158 <main+38>:     0x0f    0x05    0xcc    0xe8    0xe2    0xff    0xff    0xff
    0x401160 <main+46>:     0x48    0x65    0x6c    0x6c    0x6f    0x20    0x58    0x58
    0x401168 <main+54>:     0x58    0x20    0x57    0x6f    0x72    0x6c    0x64    0x0a
    0x401170 <main+62>:     0x00

  now, insert this hex instructions into the ptracer.c, this results to the
  following

  unsigned char insertcode[] = { 0xeb, 0x19, 0x5e, 0x48, 0xc7, 0xc0, 0x01, 0x00, 0x00, 0x00, 0x48, 0xc7, 0xc7, 0x02, 0x00, 0x00, 0x00, 0x48, 0xc7, 0xc2, 0x12, 0x00, 0x00, 0x00, 0x0f, 0x05, 0xcc, 0xe8, 0xe2, 0xff, 0xff, 0xff, 0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x58, 0x58, 0x58, 0x20, 0x57, 0x6f, 0x72, 0x6c, 0x64, 0x0a, 0x00 };


  RESOURCES
  * https://www.linuxjournal.com/article/6210 of Pradeep Padala, email: ppadala@cise.ufl.edu
  * https://stackoverflow.com/questions/23020380/inline-assembly-in-c-program-on-x86-64-linux
  * https://www.ibiblio.org/gferg/ldp/GCC-Inline-Assembly-HOWTO.html
  * https://gcc.gnu.org/onlinedocs/gcc/Using-Assembly-Language-with-C.html#Using-Assembly-Language-with-C
 */

#include <stdlib.h>
#include <unistd.h>
#include <syscall.h>

#define STDOUT 1
#define STDERR 2

int main()
{
	sleep(3);

	{
		__asm__ __volatile__(
			"   jmp .forward;"
			".backward: popq %%rsi;"
			"   movq $1, %%rax;"
			"   movq $2, %%rdi;"
			"   movq $18, %%rdx;"
			"   syscall;"
			"   int3;" // NOTE: w/o this SIGTRAP, the asm code falls through, and loops forever
			".forward: call .backward;"
			"   .string \"Hello XXX World\\n\\0\";"
			:
			:
			: "memory");
	}

	exit(EXIT_SUCCESS);
}
