/*
  ptrace example: "do something funny"

  fork child with PTRACE_TRACEME

  let it execute a command, e.g. "/bin/pwd"

  in the parent read out the RAX (EAX) register with PTRACE_PEEKUSER, aligned to 8

  if the RAX (EAX) register shows the syscall SYS_write (i.e. write to a stream)

  read all syscall registers with PTRACE_GETREGS

  show the contents of the registers rdi, rsi, rdx, r10, r8, r9

  peek the content of the SYS_write (address stored in rsi), using PTRACE_PEEKDATA

  revert the content

  poke the content of the SYS_write using PTRACE_POKEDATA


  ---


  take the output string of a child process (PTRACE_TRACEME), using
  PTRACE_PEEKDATA, revert it, and inject the reverted back into the child with
  PTRACE_POKEDATA

  the proggy forks of a 'pwd' shell command, it then catches a SYS_write call of
  the 'pwd' and kidnaps its argument (the current path) to be writen to stdout
  (rdi == 1); then it reverts the argument as string and re-injects
  (PTRACE_POKEDATA) the reverted string, which then goes to be displayed

  NOTE: the implementation is for x86_64 only!


  AUTHOR: Lothar Rubusch, L.Rubusch@gmx.ch


  RESOURCES:
  * Linux Journal, Nov 30, 2002  By Pradeep Padala ppadala@cise.ufl.edu or p_padala@yahoo.com
*/

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/reg.h>
#include <sys/syscall.h>
#include <sys/user.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int long_size = sizeof(long);

void get_data(pid_t child, long addr, char *str, int len)
{
	char *laddr;
	int idx, jdx;
	// data structure for reversing the string
	union u {
		long val;
		char chars[long_size];
	} data;

	idx = 0;
	jdx = len / long_size;
	laddr = str;
	while (idx < jdx) {
		// get 'val' for data
		data.val = ptrace(PTRACE_PEEKDATA, child, addr + idx * 8, NULL);

		// copy over to 'chars'
		memcpy(laddr, data.chars, long_size);

		// increment read index
		++idx;

		// increment write position - move pointer
		laddr += long_size;
	}

	// copy over remaining characters to 'chars'
	jdx = len % long_size;
	if (jdx != 0) {
		data.val = ptrace(PTRACE_PEEKDATA, child, addr + idx * 8, NULL);
		memcpy(laddr, data.chars, jdx);
	}
	str[len] = '\0';
}

void reverse(char *str)
{
	int idx, jdx;
	char temp;
	// NOTE: keep '-2' as upper bound, due to '\0' and '\n'!!
	for (idx = 0, jdx = strlen(str) - 2; idx <= jdx; ++idx, --jdx) {
		temp = str[idx];
		str[idx] = str[jdx];
		str[jdx] = temp;
	}
}

void put_data(pid_t child, long addr, char *str, int len)
{
	char *laddr;
	int idx, jdx;
	union u {
		long val;
		char chars[long_size];
	} data;

	idx = 0;
	jdx = len / long_size;
	laddr = str;
	while (idx < jdx) {
		// copy 'chars'
		memcpy(data.chars, laddr, long_size);

		// write from 'val'
		ptrace(PTRACE_POKEDATA, child, addr + idx * 8, data.val);

		// next to read
		++idx;

		// next to write
		laddr += long_size;
	}

	// remaining characters...
	jdx = len % long_size;
	if (jdx != 0) {
		// copy value to 'chars'
		memcpy(data.chars, laddr, jdx);

		//  print
		ptrace(PTRACE_POKEDATA, child, addr + idx * 8, NULL);
	}
}

int main(int argc, char **argv)
{
	pid_t child;
#ifndef __x86_64__
	fprintf(stderr, "Source needs x86_64 to run!!\n");
	exit(EXIT_FAILURE);
#endif

	if (0 > (child = fork())) {
		perror("fork() failed");

	} else if (0 == child) {
		/* tracee */
		ptrace(PTRACE_TRACEME, 0, NULL, NULL);
		execl("/bin/pwd", "pwd", NULL);

	} else {
		/* tracer */
		int toggle = 0;
		char *str;

		// If a tracer sets this flag, a SIGKILL signal will be sent to every tracee
		// if the tracer exits. This option is useful for ptrace jailers that want
		// to ensure that tracees can never escape the tracer's control.
		ptrace(PTRACE_SETOPTIONS, child, 0, PTRACE_O_EXITKILL);
		while (1) {
			if (0 > (waitpid(child, 0, 0))) {
				perror("waitpid: failed");
				break;
			}

			struct user_regs_struct regs;
			if (0 > (ptrace(PTRACE_GETREGS, child, 0, &regs))) {
				perror("ptrace: PTRACE_GETREGS failed");
				break;
			}

			if (SYS_write == regs.orig_rax) {
				if (0 == toggle) {
					// turn peek-reverse-poke off
					toggle = 1;

					// allocation
					str = (char *)calloc((regs.rdx + 1),
							     sizeof(char));

					// get data
					get_data(child, regs.rsi, str,
						 regs.rdx);
					fprintf(stderr,
						"\tsyscall: 0x%lx( 0x%08lx [rdi], 0x%08lx [rsi], '%s' [rdx], 0x%08lx [r10], 0x%08lx [r8], 0x%08lx [r9]\n\n",
						(long)regs.orig_rax,
						(long)regs.rdi, (long)regs.rsi,
						str, (long)regs.r10,
						(long)regs.r8, (long)regs.r9);

					// reverse
					reverse(str);
					fprintf(stderr,
						"\treverting: str = '%s'\n\n",
						str);

					// print data
					put_data(child, regs.rsi, str,
						 regs.rdx);

					free(str);
					str = NULL;

				} else {
					// turn on peek-reverse-poke of this if-cluase
					toggle = 0;

					// append linefeed
					puts("");
				}
			}
			ptrace(PTRACE_SYSCALL, child, NULL, NULL);

			// execute once, then toggle is '1'
			// if this does not break, the following error will occur
			// ptrace: PTRACE_GETREGS failed: No such process
			if (toggle)
				break;
		}
	}
	puts("READY.");

	exit(EXIT_SUCCESS);
}
