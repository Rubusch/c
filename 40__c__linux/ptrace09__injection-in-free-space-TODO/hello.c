/*
  hello world example to figure out the injection code for a ptracer

  compile:
  $ gcc -o hello.exe hello.c

  run
  gdb hello.exe
  (gdb) disassemble hello.exe
TODO                  
 */
void main()
{
  __asm__("\
         jmp forward\n\
backward:\n\
         popl   %esi\n\
         movl   $4, %eax\n\
         movl   $2, %ebx\n\
         movl   %esi, %ecx\n\
         movl   $12, %edx\n\
         int    $0x80\n\
         int3\n\
forward:\n\
         call   backward\n\
         .string \"Hello World\\n\"\n\
       ");
}
