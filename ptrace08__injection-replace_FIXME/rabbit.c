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

  ---

  disassembling

  compile it run it in gdb and extract the ASM sequence for the trap as hex

  compile:
  $ gcc -o hello.exe hello.c

  run code in gdb

  $ gdb ./hello.exe
  GNU gdb (GDB) 7.4.1-debian
  Copyright (C) 2012 Free Software Foundation, Inc.
  License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
  This is free software: you are free to change and redistribute it.
  There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
  and "show warranty" for details.
  This GDB was configured as "i486-linux-gnu".
  For bug reporting instructions, please see:
  <http://www.gnu.org/software/gdb/bugs/>...
  Reading symbols from /data/opt/git_c/ptrace08__injection-replace_FIXME/hello.exe...done.


  disassemble the assembly methods

  (gdb) disassemble main
  Dump of assembler code for function main:
  0x0804841c <+0>:     push   %ebp
  0x0804841d <+1>:     mov    %esp,%ebp
  0x0804841f <+3>:     jmp    0x8048436 <forward>
  0x08048421 <+5>:     pop    %esi
  0x08048422 <+6>:     mov    $0x4,%eax
  0x08048427 <+11>:    mov    $0x2,%ebx
  0x0804842c <+16>:    mov    %esi,%ecx
  0x0804842e <+18>:    mov    $0xc,%edx
  0x08048433 <+23>:    int    $0x80
  0x08048435 <+25>:    int3
  0x08048436 <+0>:     call   0x8048421 <main+5>
  0x0804843b <+5>:     dec    %eax
  0x0804843c <+6>:     gs
  0x0804843d <+7>:     insb   (%dx),%es:(%edi)
  0x0804843e <+8>:     insb   (%dx),%es:(%edi)
  0x0804843f <+9>:     outsl  %ds:(%esi),(%dx)
  0x08048440 <+10>:    and    %dl,0x6f(%edi)
  0x08048443 <+13>:    jb     0x80484b1 <__libc_csu_init+81>
  0x08048445 <+15>:    or     %fs:(%eax),%al
  0x08048448 <+18>:    pop    %ebp
  0x08048449 <+19>:    ret
  End of assembler dump.

  (gdb) disassemble forward
  Dump of assembler code for function main:
  0x0804841c <+0>:     push   %ebp
  0x0804841d <+1>:     mov    %esp,%ebp
  0x0804841f <+3>:     jmp    0x8048436 <forward>
  0x08048421 <+5>:     pop    %esi
  0x08048422 <+6>:     mov    $0x4,%eax
  0x08048427 <+11>:    mov    $0x2,%ebx
  0x0804842c <+16>:    mov    %esi,%ecx
  0x0804842e <+18>:    mov    $0xc,%edx
  0x08048433 <+23>:    int    $0x80
  0x08048435 <+25>:    int3
  0x08048436 <+0>:     call   0x8048421 <main+5>
  0x0804843b <+5>:     dec    %eax
  0x0804843c <+6>:     gs
  0x0804843d <+7>:     insb   (%dx),%es:(%edi)
  0x0804843e <+8>:     insb   (%dx),%es:(%edi)
  0x0804843f <+9>:     outsl  %ds:(%esi),(%dx)
  0x08048440 <+10>:    and    %dl,0x6f(%edi)
  0x08048443 <+13>:    jb     0x80484b1 <__libc_csu_init+81>
  0x08048445 <+15>:    or     %fs:(%eax),%al
  0x08048448 <+18>:    pop    %ebp
  0x08048449 <+19>:    ret
  End of assembler dump.

  (gdb) disassemble backward
  Dump of assembler code for function main:
  0x0804841c <+0>:     push   %ebp
  0x0804841d <+1>:     mov    %esp,%ebp
  0x0804841f <+3>:     jmp    0x8048436 <forward>
  0x08048421 <+5>:     pop    %esi
  0x08048422 <+6>:     mov    $0x4,%eax
  0x08048427 <+11>:    mov    $0x2,%ebx
  0x0804842c <+16>:    mov    %esi,%ecx
  0x0804842e <+18>:    mov    $0xc,%edx
  0x08048433 <+23>:    int    $0x80
  0x08048435 <+25>:    int3
  0x08048436 <+0>:     call   0x8048421 <main+5>
  0x0804843b <+5>:     dec    %eax
  0x0804843c <+6>:     gs
  0x0804843d <+7>:     insb   (%dx),%es:(%edi)
  0x0804843e <+8>:     insb   (%dx),%es:(%edi)
  0x0804843f <+9>:     outsl  %ds:(%esi),(%dx)
  0x08048440 <+10>:    and    %dl,0x6f(%edi)
  0x08048443 <+13>:    jb     0x80484b1 <__libc_csu_init+81>
  0x08048445 <+15>:    or     %fs:(%eax),%al
  0x08048448 <+18>:    pop    %ebp
  0x08048449 <+19>:    ret
  End of assembler dump.


  we need to take the machine code bytes from main+3 to backward+20, which is a
  total of 41 bytes; obtain this sequence as HEX

  (gdb) x/40bx main+3
  0x804841f <main+3>:     0xeb    0x15    0x5e    0xb8    0x04    0x00    0x00    0x00
  0x8048427 <main+11>:    0xbb    0x02    0x00    0x00    0x00    0x89    0xf1    0xba
  0x804842f <main+19>:    0x0c    0x00    0x00    0x00    0xcd    0x80    0xcc    0xe8
  0x8048437 <forward+1>:  0xe6    0xff    0xff    0xff    0x48    0x65    0x6c    0x6c
  0x804843f <forward+9>:  0x6f    0x20    0x57    0x6f    0x72    0x6c    0x64    0x0a

  now, insert this hex instructions into the ptracer.c, this results to the following

  "\xeb\x15\x5e\xb8\x04\x00"
  "\x00\x00\xbb\x02\x00\x00\x00\x89\xf1\xba"
  "\x0c\x00\x00\x00\xcd\x80\xcc\xe8\xe6\xff"
  "\xff\xff\x48\x65\x6c\x6c\x6f\x20\x57\x6f"
  "\x72\x6c\x64\x0a\x00"
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
