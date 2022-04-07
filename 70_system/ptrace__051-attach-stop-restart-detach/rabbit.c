/*
  modified hello world from before...

  after sleeping for 10 sec the rabbit.exe is going to enter its asm instructions
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



  resources:
  jim fisher  https://jameshfisher.com/2018/02/20/c-inline-assembly-hello-world
  https://www.ibiblio.org/gferg/ldp/GCC-Inline-Assembly-HOWTO.html
  https://stackoverflow.com/questions/23020380/inline-assembly-in-c-program-on-x86-64-linux
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

	sleep(1); // setup for ptrace

	// initializing the assembler
	register int syscall_no asm("rax") = SYS_write; /* syscall number */
	register int arg1 asm("rdi") = STDERR; /* destination: the std stream */
	register char *arg2 asm("rsi") = msg; /* source, the message */
	register int arg3 asm("rdx") = msg_size; /* size of source */

	// execute assembly
	asm("syscall");

	exit(EXIT_SUCCESS);
}
