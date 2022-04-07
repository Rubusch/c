/*
  Hello World

  after sleeping for 3 sec the rabbit.exe is going to enter its asm instructions
  it falls into trap and cannot end only when ptracer.exe
  receives enter, it leaves the attached rabbit.exe

  example prepared for code injection in my ptracer,
  the NASM (Net-ASM) assembler syntax was translated to GAS (GCC Assembler
  Syntax), for compiling it as inlined __asm__ source inside linux/c gcc



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
  $ gcc -o rabbit.exe rabbit.c
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
       0x0000000000001145 <+0>:     push   %rbp
       0x0000000000001146 <+1>:     mov    %rsp,%rbp
       0x0000000000001149 <+4>:     sub    $0x10,%rsp
       0x000000000000114d <+8>:     movl   $0xe,-0x4(%rbp)
       0x0000000000001154 <+15>:    movq   $0x0,-0x10(%rbp)
       0x000000000000115c <+23>:    mov    $0x3,%edi
       0x0000000000001161 <+28>:    callq  0x1040 <sleep@plt>
       0x0000000000001166 <+33>:    lea    0x2ecb(%rip),%rcx        # 0x4038 <msg.3132>
       0x000000000000116d <+40>:    mov    -0x4(%rbp),%eax
       0x0000000000001170 <+43>:    mov    %eax,%edx
       0x0000000000001172 <+45>:    mov    $0x1,%rax
       0x0000000000001179 <+52>:    mov    $0x2,%rdi
       0x0000000000001180 <+59>:    mov    %rcx,%rsi
       0x0000000000001183 <+62>:    syscall
       0x0000000000001185 <+64>:    mov    %rax,-0x10(%rbp)
       0x0000000000001189 <+68>:    mov    $0x0,%edi
       0x000000000000118e <+73>:    callq  0x1030 <exit@plt>
    End of assembler dump.

  (gdb) disassemble sleep
    Dump of assembler code for function sleep@plt:
       0x0000000000001040 <+0>:     jmpq   *0x2fda(%rip)        # 0x4020 <sleep@got.plt>
       0x0000000000001046 <+6>:     pushq  $0x1
       0x000000000000104b <+11>:    jmpq   0x1020
    End of assembler dump.


  RESOURCES
  * jim fisher  https://jameshfisher.com/2018/02/20/c-inline-assembly-hello-world
  * https://www.ibiblio.org/gferg/ldp/GCC-Inline-Assembly-HOWTO.html
  * https://stackoverflow.com/questions/23020380/inline-assembly-in-c-program-on-x86-64-linux
 */

#include <stdlib.h>
#include <unistd.h>
#include <syscall.h>

#define STDOUT 1
#define STDERR 2

int main()
{
	static char msg[] = "Hello World!\n";
	int msg_size = sizeof(msg);
	long res = 0;

	sleep(3); // setup for ptrace

	/*
  // variant I)

  // initializing the assembler
  // NOTE: this will need '-Wno-unused-variable' in a Makefile
  register int    syscall_no  asm("rax") = SYS_write; // syscall number
  register int    arg1        asm("rdi") = STDERR;    // destination: the std stream
  register char*  arg2        asm("rsi") = msg;       // source, the message
  register int    arg3        asm("rdx") = msg_size;  // size of source

  // execute assembly
  asm("syscall");
// */

	//*
	// variant II)

	// assembly initialization
	{
		__asm__ __volatile__("syscall"
				     : "=a"(res)
				     : "a"(SYS_write), "D"(STDOUT), "S"(msg),
				       "d"(msg_size)
				     : "memory");
	}
	// */

	/*
  // variant III)

  // assembly instructions
  {
    __asm__ __volatile__ (
                          "movq $1, %%rax\n\t"
                          "movq $2, %%rdi\n\t"
                          "movq %1, %%rsi\n\t"
                          "syscall\n\t"
                          : "=a" (res)
                          : "r"(msg), "d"(msg_size)
                          : "memory"
                          );
  }
// */

	exit(EXIT_SUCCESS);
}
