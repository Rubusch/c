/*
  Inline ASM Demo

  Implementation of a syscall. In Linux, system calls are implemented using
  GCC inline assembly. All the system calls are written as macros
  (linux/unistd.h). The current example shows a syscall e.g. 'write' with three
  arguments.
  A process is forked, and among the two resulting processes a pipe is opened.
  The parent now sends a message over the pipe, using the syscall interface for
  writing onto the pipe. The child reads the pipe and displays the received
  string.



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

  NOTE: To specify the register, you must directly specify the register
  names by using specific register constraints. They are:

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

  NOTE: further constraints
  Some other constraints used are:

  "m" : A memory operand is allowed, with any kind of address that the machine
        supports in general.
  "o" : A memory operand is allowed, but only if the address is offsettable. ie,
        adding a small offset to the address gives a valid address.
  "V" : A memory operand that is not offsettable. In other words, anything that
        would fit the 'm' constraint but not the 'o' constraint.
  "i" : An immediate integer operand (one with constant value) is allowed. This
        includes symbolic constants whose values will be known only at assembly
        time.
  "n" : An immediate integer operand with a known numeric value is allowed. Many
        systems cannot support assembly-time constants for operands less than a
        word wide. Constraints for these operands should use 'n' rather than
        'i'.
  "g" : Any register, memory or immediate integer operand is allowed, except for
        registers that are not general registers.

Following constraints are x86 specific.

    "r" : Register operand constraint, look table given above.
    "q" : Registers a, b, c or d.
    "I" : Constant in range 0 to 31 (for 32-bit shifts).
    "J" : Constant in range 0 to 63 (for 64-bit shifts).
    "K" : 0xff.
    "L" : 0xffff.
    "M" : 0, 1, 2, or 3 (shifts for lea instruction).
    "N" : Constant in range 0 to 255 (for out instruction).
    "f" : Floating point register
    "t" : First (top of stack) floating point register
    "u" : Second floating point register
    "A" : Specifies the 'a' or 'd' registers. This is primarily useful for
          64-bit integer values intended to be returned with the `d’ register
          holding the most significant bits and the 'a' register holding the
          least significant bits.

  author: Lothar Rubusch, l.rubusch@gmx.ch

  resource:
  https://www.ibiblio.org/gferg/ldp/GCC-Inline-Assembly-HOWTO.html
 */

#define _POSIX_SOURCE
#include <syscall.h> /* SYS_write */
#include <unistd.h> /* ssize_t */

#include <asm/unistd.h> /* __NR_write */
#include <unistd.h> /* write */
#include <sys/wait.h> /* pid_t */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* // for completeness, a version for 32 bit assembler:
#define my_syscall3(type, NAME, type1, arg1, type2, arg2, type3, arg3)  \
  type NAME(type1 arg1, type2 arg2, type3 arg3)                         \
  {                                                                     \
    long __res;                                                         \
    __asm__ volatile (  "int $0x80"                                     \
                        : "=a" (__res)                                  \
                        : "0" (__NR_##NAME),"b" ((long)(arg1)),"c" ((long)(arg2)), \
                          "d" ((long)(arg3)));                          \
    return (type) __res;                                                \
  }
// interrupt 0x80 is plain old 32 bit assembly,
// on x86_64 the 'syscall' command should be used,
// but the mechanism will be different
*/

// NOTE: syscalls on x86_64 take arguments in the following general purpose registers:
// rdi, rsi, rdx, r10, r8, r9 (last three are not used here)
#define my_syscall3(type, NAME, syscall_number, type1, rdi, type2, rsi, type3, rdx) \
  type NAME(type1 rdi, type2 rsi, type3 rdx)                            \
  {                                                                     \
    long __res;                                                         \
    register long r10 __asm__("r10") = 0;                               \
    register long r8 __asm__("r8") = 0;                                 \
    register long r9 __asm__("r9") = 0;                                 \
    __asm__ volatile (  "syscall"                                       \
                        : "=a" (__res)                                  \
                        : "a" (syscall_number), "D" (rdi), "S" (rsi), "d" (rdx), "r" (r10), "r" (r8), "r" (r9) \
                        );                                              \
    return (type) __res;                                                \
  }
// */

// define syscall function
my_syscall3(ssize_t, plop, SYS_write, int, fildes, const char*, buf, size_t, nbyte)


int main(void)
{
  int fd[2]; // fd[0]: read - fd[1]: write
  pid_t pid;

  pipe(fd);
  pid = fork();
  if (0 > pid) {
    perror("fork failed");

  } else if (0 < pid) {
    /* parent */
    close(fd[0]); // close reading end

    static const char msg[] = "Volare, oh, oh - Cantare, oh, oh, oh, oh";
    fprintf(stderr, "message: '%s'\n", msg);
    // here we would write the pipe from parent to child, let's take our own system call in GAS
    // originally: write(fd[1], msg, sizeof(msg));
    plop(fd[1], msg, sizeof(msg));
    wait(NULL); // wait for child to send something
    close(fd[1]);

  } else {
    /* child */
    char copy[BUFSIZ]; memset(copy, '\0', BUFSIZ);
    close(fd[1]); // close writing end
    read(fd[0], copy, BUFSIZ);
    fprintf(stderr, "received: '%s'\n", copy);
    close(fd[0]);
    puts("READY.");
  }

  exit(EXIT_SUCCESS);
}
