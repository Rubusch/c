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



  GCC inline assembly

  Here is attempt to make best notes about gcc inline asm. As this is just small
  operator that is for long time, i dont thing there is some full guide how to
  use it lets do it here for wisdom of internet.


  Inline assembly syntax

  asm [volatile] (
          AssemblerTemplate
          : OutputOperands
          [ : InputOperands
          [ : Clobbers ] ]
  )


  Output operands

  Option   Note
  -------------------------------------------------
  "=r"     Write output to register


  Input operands

  Option   Note
  -------------------------------------------------
  "r"      Input to register


  Clobbers used to tell GCC that registers are used.

  Clobber   option Note
  -------------------------------------------------
  "0"       map to register "0" same register


  Modifier Specifies

  = Write-only operand, usually used for all output operands
  + Read-write operand, must be listed as an output operand
  & A register that should be used for output only


  Table of AMD64 register names

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


  Register  Name
  -------------------------------------------------
  rax       %%rax
  rbx       %%rbx
  rcx       %%rcx
  rdx       %%rdx
  rdi       %%rdi
  rsi       %%rsi
  rbp       %%rbp
  rsp       %%rsp
  r8        %%r8
  r9        %%r9
  r10       %%r10
  r11       %%r11
  r12       %%r12
  r13       %%r13
  r14       %%r14
  r15       %%r15


  Syscall calling convention - Intel AMD64 Linux

  Param Register
  -------------------------------------------------
  1th   rdi
  2th   rsi
  3th   rdx
  4th   rcx
  5th   r8
  6th   r9

  syscall number rax
  TODO r10?

  Examples

  AMD64 Add two numbers

  int32_t a=1,b=2,c=-1;
  asm(
      "movl %1, %0\n\t"
      "addl %2, %0\n\t"
      :"=r"(c)
      :"r"(a),"r"(b)
      :"0");

  a,b - use regisers and save result c to register, make to use for c same
  register by mentioning "0" in clobber register


  Output:
      movl %edx, %edx
      addl %ecx, %edx


  int32_t a=1,b=2,c=-1;
  asm(
      "movl %1, %0\n\t"
      "addl %2, %0\n\t"
      :"=r"(c)
      :"g"(a),"g"(b)
      :"0");

  Output:
      movl -4(%rbp), %edx
      addl -8(%rbp), %edx


  AMD64 Call linux syscall mmap with inline asm

  long sys_mmap(unsigned long addr, unsigned long len, unsigned long prot,
  unsigned long flags, unsigned long fd, unsigned long off)
  {
      long ret;
      asm(
          "mov %p6, %%r9\n\t"
          "mov %p5, %%r8\n\t"
          "mov %p4, %%r10\n\t"
          "mov %p3, %%rdx\n\t"
          "mov %p2, %%rsi\n\t"
          "mov %p1, %%rdi\n\t"
          "mov $9, %%rax\n\t"
          "syscall\n\t"
          :"=a"(ret)
          :[p1]"m"(addr),[p2]"m"(len),[p3]"m"(prot),[p4]"m"(flags),[p5]"m"(fd),[p6]"m"(off));
      return ret;
  }

  Put result of execution to ret, all paramters in memory


  Intel random number with RDRAND

  uint64_t get_hw_rand()
  {
      uint64_t ret;
      int i=0;
      const int timeout = 10;
      while (i<timeout)
      {
          asm("rdrand %0\n\t"
              :"=a"(ret)::);
          if (ret) break;
          i++;
      }
      return ret;
  }


  Switching on of intel/att syntax

  Inline assembler for GCC by default uses AT&T syntax. There is possible to
  turn on/off intel syntax.

  asm(".intel_syntax noprefix");
  asm("mov eax, 1");
  asm(".att_syntax prefix");



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



  RESOURCES
  http://main.lv/writeup/gcc_inline_assembly.md
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
