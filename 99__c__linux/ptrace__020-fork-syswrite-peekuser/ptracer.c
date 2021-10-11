/*
  ptrace example

  fork child with PTRACE_TRACEME

  let it execute a command, e.g. "/bin/pwd"

  in the parent read out the RAX (EAX) register with PTRACE_PEEKUSER, aligned to 8

  if the RAX (EAX) register shows the syscall SYS_write (i.e. write to a stream)

  read the RBX, RCX and RDX registers individually

  show the contents of the RBX, RCX and RDX registers


  ---

  ptrace is a system call found in Unix and several Unix-like operating systems.
  By using ptrace (the name is an abbreviation of "process trace") one process
  can control another, enabling the controller to inspect and manipulate the
  internal state of its target. ptrace is used by debuggers and other
  code-analysis tools, mostly as aids to software development.
  (wikipedia)

  tracking system calls - when a write syscall was caught in the child, ptrace
  reads out child's registers %ebx, %ecx and %edx one by one, else it prints
  the %eax register for other syscalls

  peek registers directly via PTRACE_PEEKUSER

  NOTE: for x86_64 the alignment is 8, thus registers need to be multiplied by 8


  AUTHOR: Lothar Rubusch, L.Rubusch@gmx.ch


  RESOURCES:
  * wikipedia
  * Linux Journal, Nov 30, 2002  By Pradeep Padala ppadala@cise.ufl.edu or p_padala@yahoo.com
*/

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>
#if __x86_64__
#include <sys/reg.h>
#else
#include <asm/ptrace-abi.h> /* constants, e.g. ORIG_EAX, etc. */
#endif
#include <sys/syscall.h> /* SYS_write */

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	pid_t child;
	long orig_eax, eax;
	long register_arguments[3];
	int status;
	int insyscall = 0;

	if (0 > (child = fork())) {
		perror("fork() failed");
	} else if (0 == child) {
		/* tracee */
		ptrace(PTRACE_TRACEME, 0, NULL, NULL);
		execl("/bin/pwd", "pwd", NULL);

	} else {
		/* tracer */
		while (1) {
			// check wether the child was stopped by ptrace or exited
			wait(&status);
			if (WIFEXITED(status))
				break;

#if __x86_64__
			orig_eax = ptrace(PTRACE_PEEKUSER, child, 8 * ORIG_RAX,
					  NULL);

			// tracking the 'write' syscall
			if (orig_eax == SYS_write) {
				if (insyscall == 0) {
					// syscall entry: PTRACE_PEEKUSER looks into the arguments of the child
					insyscall = 1;
					register_arguments[0] =
						ptrace(PTRACE_PEEKUSER, child,
						       8 * RBX, NULL);
					register_arguments[1] =
						ptrace(PTRACE_PEEKUSER, child,
						       8 * RCX, NULL);
					register_arguments[2] =
						ptrace(PTRACE_PEEKUSER, child,
						       8 * RDX, NULL);
					fprintf(stderr,
						"parent: write called with 0x%08lx [rbx], 0x%08lx [rcx], 0x%08lx [rdx]\n",
						register_arguments[0],
						register_arguments[1],
						register_arguments[2]);
				} else {
					// syscall exit
					eax = ptrace(PTRACE_PEEKUSER, child,
						     8 * RAX, NULL);
					fprintf(stderr,
						"parent: write returned with 0x%08lx [rax]\n",
						eax);
					insyscall = 0;
				}
			}
#else
			orig_eax = ptrace(PTRACE_PEEKUSER, child, 4 * ORIG_EAX,
					  NULL);

			// tracking the 'write' syscall
			if (orig_eax == SYS_write) {
				if (insyscall == 0) {
					// syscall entry: PTRACE_PEEKUSER looks into the arguments of the child
					insyscall = 1;
					register_arguments[0] =
						ptrace(PTRACE_PEEKUSER, child,
						       4 * EBX, NULL);
					register_arguments[1] =
						ptrace(PTRACE_PEEKUSER, child,
						       4 * ECX, NULL);
					register_arguments[2] =
						ptrace(PTRACE_PEEKUSER, child,
						       4 * EDX, NULL);
					fprintf(stderr,
						"parent: write called with 0x%04lx [ebx], 0x%04lx [ecx], 0x%04lx [edx]\n",
						register_arguments[0],
						register_arguments[1],
						register_arguments[2]);
				} else {
					// syscall exit
					eax = ptrace(PTRACE_PEEKUSER, child,
						     4 * EAX, NULL);
					fprintf(stderr,
						"parent: write returned with 0x%04lx [eax]\n",
						eax);
					insyscall = 0;
				}
			}
#endif

			// stop the child process whenever a syscall entry/exit was received
			ptrace(PTRACE_SYSCALL, child, NULL, NULL);
		}
	}
	return 0;
}
