/*
  hello.c - simple hello world demo
 */



void
main(int argc, char** argv)
{
__asm__("
        jmp forward
backward:
        popl %esi          # get addr of hellow world string
        movl $4, %eax       # do 'write' sys call
        movl $2, %ebx
        movl %esi, %ecx
        movl $12, %edx
        int  $0x80
        int3                # breakpoint - here the program
                            # will stop and give control back
                            # to parent
forward:
        call backward
        .string \"hello world\\n\"
"
	);
}

