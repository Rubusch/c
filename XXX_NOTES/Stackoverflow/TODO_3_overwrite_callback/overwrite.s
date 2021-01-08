	.file	"overwrite.c"
	.text
.globl sp
	.type	sp, @function
sp:
	pushl	%ebp
	movl	%esp, %ebp
#APP
	movl %esp,%eax
#NO_APP
	popl	%ebp
	ret
	.size	sp, .-sp
	.section	.rodata
.LC0:
	.string	"*tmp\t= 0x%lx - **tmp = 0x%lx\n"
	.text
.globl func
	.type	func, @function
func:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$40, %esp
	leal	-13(%ebp), %eax
	movl	%eax, -8(%ebp)
	addl	$53, -8(%ebp)
	movl	-8(%ebp), %edx
	movl	-4(%ebp), %eax
	movl	%edx, (%eax)
	movl	-4(%ebp), %eax
	movl	(%eax), %eax
	leal	212(%eax), %edx
	movl	-4(%ebp), %eax
	movl	%edx, (%eax)
	movl	-4(%ebp), %eax
	movl	(%eax), %eax
	movl	(%eax), %edx
	movl	-4(%ebp), %eax
	movl	(%eax), %eax
	movl	%edx, 8(%esp)
	movl	%eax, 4(%esp)
	movl	$.LC0, (%esp)
	call	printf
	leave
	ret
	.size	func, .-func
	.section	.rodata
.LC1:
	.string	"stack\t= 0x%lx\n"
.LC2:
	.string	"&x\t= 0x%lx - x = %d\n"
	.text
.globl main
	.type	main, @function
main:
	leal	4(%esp), %ecx
	andl	$-16, %esp
	pushl	-4(%ecx)
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ecx
	subl	$36, %esp
	call	sp
	movl	%eax, 4(%esp)
	movl	$.LC1, (%esp)
	call	printf
	movl	$0, -8(%ebp)
	movl	-8(%ebp), %edx
	leal	-8(%ebp), %eax
	movl	%edx, 8(%esp)
	movl	%eax, 4(%esp)
	movl	$.LC2, (%esp)
	call	printf
	movl	$3, 8(%esp)
	movl	$2, 4(%esp)
	movl	$1, (%esp)
	call	func
	movl	$1, -8(%ebp)
	movl	-8(%ebp), %edx
	leal	-8(%ebp), %eax
	movl	%edx, 8(%esp)
	movl	%eax, 4(%esp)
	movl	$.LC2, (%esp)
	call	printf
	movl	$0, (%esp)
	call	exit
	.size	main, .-main
	.ident	"GCC: (GNU) 4.2.3 20071014 (prerelease) (Debian 4.2.2-3)"
	.section	.note.GNU-stack,"",@progbits
