/*
  example of single stepping through code using ptrace

  author: Lothar Rubusch
  email: L.Rubusch@gmx.ch
  original: Linux Gazette, http://linuxgazette.net/issue81/sandeep.html, By Sandeep S, sk_nellayi@rediffmail.com
*/

#include <stdio.h>
#include <stdlib.h>

//#include <signal.h>

#include <syscall.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

//#include <asm/ptrace-abi.h> /* constants, e.g. ORIG_EAX, etc. */ 
//#include <sys/syscall.h> 
#include <sys/user.h> /* struct user_regs_struct */

#include <errno.h>


int
main(void)
{
	struct user_regs_struct regs;  /* read out registers */
	long long counter = 0;     /*  machine instruction counter */
	
	int status;                    /*  child's return value        */
	int pid;                       /*  child's process id          */

	puts("Please wait");

	switch (pid = fork()) {
	case -1:
		perror("fork");
		break;
	case 0: /*  child process starts */
		ptrace(PTRACE_TRACEME, 0, NULL, NULL);
		/*
		 *  must be called in order to allow the
		 *  control over the child process
		 */
		execl("/bin/ls", "ls", NULL);
//		execl("/opt/git_c/ptrace__singlestep01/rabbit.exe", "rabbit.exe", NULL);
		/*
		 *  executes the program and causes
		 *  the child to stop and send a signal
		 *  to the parent, the parent can now
		 *  switch to PTRACE_SINGLESTEP
		 */
		break; /* never reached */

	default: /*  parent process starts */
		wait(&status);
		if (WIFEXITED(status)) {
			break;
		}
		/*
		 *   parent waits for child to stop at next
		 *   instruction (execl())
		 */
// TODO check val 1407                                         
		while (status == 1407 ) {
			counter++;
			if (ptrace(PTRACE_SINGLESTEP, pid, 0, 0) != 0){
				perror("ptrace");
			}
//			fprintf(stderr, "xxx\n");    
			/*
			 *   switch to singlestep tracing and
			 *   release child
			 *   if unable call error.
			 */

			wait(&status);
			/*   wait for next instruction to complete  */
		}
		/*
		 * continue to stop, wait and release until
		 * the child is finished; status != 1407
		 * Low=0177L and High=05 (SIGTRAP)
		 */
		printf("Number of machine instructions : %lld\n", counter);
	}

	exit(EXIT_SUCCESS);
}
