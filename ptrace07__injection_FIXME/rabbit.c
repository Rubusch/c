/*
  fabricated hello world

  example prepared for code injection for a ptracer, breakpoint will replace the following instruction:
         int    $0x80\n\

  for running the example, compile it (Makefile) and in one shell window run
  $ ./rabbit.exe

  in another shell window run, within 10 sec
  $ ./ptracer.exe `pidof rabbit.exe`

  after sleeping for 10 sec the rabbit.exe is going to enter its asm instructions
  where it falls into trap and cannot end
  only when ptracer.exe receives enter, it leaves the attached rabbit.exe

TODO check                  
  Note: rabbit.exe was stopped, w/o being a child, nor having declared PTRACE_TRACEME explicitely!

 */

#include <unistd.h>

int
main()
{
	sleep(10);

	__asm__("\
         jmp forward\n				\
backward:\n					\
         popl   %esi\n				\
         movl   $4, %eax\n			\
         movl   $2, %ebx\n			\
         movl   %esi, %ecx\n			\
         movl   $12, %edx\n			\
         int    $0x80\n				\
         int3\n					\
forward:\n					\
         call   backward\n			\
         .string \"Hello World\\n\"\n		\
       ");
	return 0;
}
