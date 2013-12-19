// debugger
/*
  @author: Lothar Rubusch
  @email: L.Rubusch@gmx.ch

  @license: EPL
  @2013-december-20
//*/

#include <signal.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/ptrace.h> 
#include <sys/reg.h> 
#include <sys/user.h> 

#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#include <dirent.h> 
#include <string.h> 
#include <signal.h> 
#include <errno.h> 
#include <stdio.h> 


#include "ptracer.h"




/* debugging */
void
printError(int errnum)
{
	// evaluating errno
	switch(errnum){
#ifdef EACCES
	case EACCES:
		perror("EACCES permission denied");
		break;
#endif
#ifdef EPERM
	case EPERM:
		perror("EPERM is not super-user");
		break;
#endif
#ifdef E2BIG
	case E2BIG:
		perror("E2BIG arg list is too long");
		break;
#endif
#ifdef ENOEXEC
	case ENOEXEC:
		perror("ENOEXEC exec format error");
		break;
#endif
#ifdef EFAULT
	case EFAULT:
		perror("EFAULT bad address");
		break;
#endif
#ifdef ENAMETOOLONG
	case ENAMETOOLONG:
		perror("ENAMETOOLONG path name is too long");
		break;
#endif
#ifdef ENOENT
	case ENOENT:
		perror("ENOENT no such file or directory");
		break;
#endif
#ifdef ENOMEM
	case ENOMEM:
		perror("ENOMEM not enough core");
		break;
#endif
#ifdef ENOTDIR
	case ENOTDIR:
		perror("ENOTDIR not a directory");
		break;
#endif
#ifdef ELOOP
	case ELOOP:
		perror("ELOOP too many symbolic links");
		break;
#endif
#ifdef ETXTBSY
	case ETXTBSY:
		perror("ETXTBSY text file busy");
		break;
#endif
#ifdef EIO
	case EIO:
		perror("EIO I/O error");
		break;
#endif
#ifdef ENFILE
	case ENFILE:
		perror("ENFILE too many open files in system");
		break;
#endif
#ifdef EINVAL
	case EINVAL:
		perror("EINVAL invalid argument");
		break;
#endif
#ifdef EISDIR
	case EISDIR:
		perror("EISDIR is a directory");
		break;
#endif
#ifdef ELIBBAD
	case ELIBBAD:
		perror("accessing a corrupted shared lib");
		break;
#endif
	default:
		fprintf(stderr, "error %i occured\r\n", errnum);
		break;
	}
}


void cleanup(){
	;
// TODO
// in case trigger a cleanup process by a SIG handler to parent, who takes care
// of child, deallocates resources, deconnects, etc.  cleanly and puts the
// parent in a "safe" state
}

/*
// TODO rm
void
suspend_handler(int signum)
{
	puts("child: suspend_handler called");
}

void
resume_handler(int signum)
{
	puts("child: resume_handler");
}
//*/

                                                                               

/* printing reginsters */
void
show_registers(FILE *const out, pid_t pid, const char *const note)
{
	struct user_regs_struct regs;
	long ins;

	do {
		ins = ptrace(PTRACE_GETREGS, pid, &regs, &regs);
	} while (ins == -1L && errno == ESRCH);
	if (ins == -1L) {
		return;
	}

#if (defined(__x86_64__) || defined(__i386__)) && __WORDSIZE == 64
        /* 64 bit */
// TODO test thoroughly
	ins = ptrace(PTRACE_PEEKTEXT, pid, regs.rip, NULL);
	fprintf(out, "instr: 0x%08lx\n", ins);
	fprintf(out, "PID %d: EIP=0x%08lxx, ESP=0x%08lx ", (int)pid, regs.rip, regs.rsp);

#elif (defined(__x86_64__) || defined(__i386__)) && __WORDSIZE == 32
        /* 32 bit */
	ins = ptrace(PTRACE_PEEKTEXT, pid, regs.eip, NULL);
	fprintf(out, "instr: 0x%08lx\n", ins);
	fprintf(out, "PID %d: EIP=0x%08lxx, ESP=0x%08lx ", (int)pid, regs.eip, regs.esp);
#endif
	if (note && *note) {
		fprintf(out, "%s\n", note);
	}
}

                                                                               


void
child( char** prog, pid_t pid_parent, void (*traceme_fun) (void) )
{
	printf("child pid: %i, parent: %i\r\n\n\n", getpid(), pid_parent);

	/* call function, in case PTRACE_TRACEME */
	(*traceme_fun) ();

	/* stop process right away */
        kill(getpid(), SIGSTOP); // currently, stop for singlestep      

	fflush(stdout);
	fflush(stderr);

	int execReturn = execvp(prog[1], prog + 1);

/*
// TODO signal listener
  how to suspend / awake child processes?

  The "graceful" one is SIGTSTP, and its purpose is to "nicely" ask the process,
  if it feels like it, to please suspend execution until it receives a SIGCONT.
  In the case of SIGTSTP, the process can ignore SIGTSTP and continue executing
  anyway, so this requires cooperation from a program that is designed to handle
  SIGTSTP.

  The "forceful" one is SIGSTOP, and its purpose is to suspend all userspace
  threads associated with that process. It is just as impossible for the process
  to ignore SIGSTOP as it is for it to ignore SIGKILL (the latter kills the
  process forcefully).
*/

	// never should reach here!
	perror("Failure! Child EXEC call returned.");
	printError(execReturn);
	cleanup();
	exit(EXIT_FAILURE);
}


// version: threader
int
tracer(pid_t pid)
{
	int status = 0;
	long ret;
	int stepwise = 0;
	int event;
	int done = 0;
					

	while (1) {
		// wait for a child event.
		if (wait_process(pid, &status)) {
			break;
		}
		// exited?
		if (WIFEXITED(status) || WIFSIGNALED(status)) {
			errno = 0;
			break;
		}
		// at this point, only stopped events are interesting.
		if (!WIFSTOPPED(status)) {
			continue;
		}


		// stop child - done by itself

//*
		// set breakpoints
		tracee_addr_t break_addr = (tracee_addr_t)0x0804847c;              
		breakpoint_t *bp = set_breakpoint(pid, break_addr);
// TODO breakpoints so far unused                                                  


		// continue until breakpoint
		do {
			ret = ptrace(PTRACE_CONT, pid, NULL, NULL);
		} while (ret == -1L && (errno == EBUSY || errno == EFAULT || errno == ESRCH));
//*/

		while (1) {

			if (!stepwise) {
/*
				// breakpoint - restore orig instructions
				int last_sig = 0;
				ptrace(PTRACE_SINGLESTEP, pid, 0, last_sig);
				waitpid(pid, &status, 0);
				
				if (WIFEXITED(status)) {
					return 0;
				}

				if (WIFSTOPPED(status)) {
					last_sig = WSTOPSIG(status);
					if (last_sig == SIGTRAP) {
						event = (status >> 16) & 0xffff;
//						return (event == PTRACE_EVENT_EXIT) ? 0 : 1;
						if (event == PTRACE_EVENT_EXIT) {
							continue;
						}
			
//			do {
						ret = ptrace(PTRACE_POKEUSER, pid, sizeof(long)*REGISTER_IP, bp->code_addr);
//			} while (ret == -1L && (errno == EBUSY || errno == EFAULT || errno == ESRCH));

			// set registers back
//			do {
						ret = ptrace(PTRACE_POKETEXT, pid, bp->code_addr, bp->code_backup);
//			} while (ret == -1L && (errno == EBUSY || errno == EFAULT || errno == ESRCH));

				
						stepwise = 1;

					}
				}
/*/
						stepwise = 1;
//*/

			} else {
				// stepwise debugging

				// user input
				int pedantic = 1;
				char chr[3];
				while (pedantic) {
					fprintf(stdout, "Process stopped, hit ENTER for next step\n> ");
					fflush(stdout);
					memset(chr, '\0', sizeof(chr));
					fgets(chr, sizeof(chr), stdin);
					switch (chr[0])
					{
					case 'c':
					case 'C':
						stepwise = 0;
						pedantic = 0;
						done = 1;
						break;
					case 'q':
					case 'Q':
						exit(EXIT_SUCCESS);
						break;
					case 'n':
					case 'N':
					case '\n': // TODO for \n repeat old command
						stepwise = 1;
						pedantic = 0;
						break;
						// TODO next step
					default:
						fprintf(stdout, "\nUsage:\n");
						fprintf(stdout, "\tq - quit\n");
						fprintf(stdout, "\th - help\n");
						fprintf(stdout, "\tc - continue\n");
						fprintf(stdout, "\tn - next instruction\n");
						fprintf(stdout, "\tENTER - last action\n"); // TODO
						break;
					}
				}

				// done?
				if (done) {
					break;
				}

				// display
				show_registers(stdout, pid, "Advanced by one step.");
				printf("\n");
				fflush(stdout);

				// singlestep
				do {
					ret = ptrace(PTRACE_SINGLESTEP, pid, NULL, NULL);
				} while (ret == -1L && errno == ESRCH);
				if (ret) {
					fprintf(stderr, "Single-step failed: %s.\n", strerror(errno));
					fflush(stderr);
				}
			}
		}

//*
                // detach
		do {
			ret = ptrace(PTRACE_DETACH, pid, (void *)0, (void *)0);   
		} while (ret == -1 && (errno == EBUSY || errno == EFAULT || errno == ESRCH));
//*/

                // get current status
		if (continue_process(pid, &status)) {
			fprintf(stderr, "Continue process failed...\n");
			break;
		}
		if (WIFCONTINUED(status)) {
			printf("Detached. Waiting for new stop events.\n\n");
			fflush(stdout);
			continue;
		}
		errno = 0;
		break;
	}

	// continuing - status of child
	if (errno) {
		fprintf(stderr, "Tracer: Child lost (%s)\n", strerror(errno));
	} else {
		if (WIFEXITED(status)) {
			fprintf(stderr, "Tracer: Child exited (%d)\n", WEXITSTATUS(status));
		} else {
			if (WIFSIGNALED(status)) {
				fprintf(stderr, "Tracer: Child died from signal %d\n", WTERMSIG(status));
			} else {
				fprintf(stderr, "Tracer: Child vanished\n");
			}
		}
	}
	fflush(stderr);

	return status;
}


static int
wait_process(const pid_t pid, int *const statusptr)
{
	int status;
	pid_t p;

	do {
		status = 0;
		p = waitpid(pid, &status, WUNTRACED | WCONTINUED);
	} while ((p == (pid_t)-1) && (errno == EINTR));
	if (p != pid) {
		return errno = ESRCH;
	}

	if (statusptr) {
		*statusptr = status;
	}

	return errno = 0;
}

static int
continue_process(const pid_t pid, int *const statusptr)
{
	int   status;
	pid_t p;

	do {
		if (kill(pid, SIGCONT) == -1) {
			fprintf(stderr, "%s (%d): kill(pid, SIGCONT) failed", __FILE__, __LINE__);
			return errno = ESRCH;
		}

		do {
			status = 0;
			p = waitpid(pid, &status, WUNTRACED | WCONTINUED);
		} while (p == (pid_t)-1 && errno == EINTR);

		if (p != pid) {
			fprintf(stderr, "%s (%d): (p != pid)", __FILE__, __LINE__);
			return errno = ESRCH;
		}

	} while (WIFSTOPPED(status));

	if (statusptr) {
		*statusptr = status;
	}

	return errno = 0;
}


breakpoint_t*
set_breakpoint(pid_t pid, tracee_addr_t addr) {
	breakpoint_t *bp = malloc(sizeof(*bp));
	bp->code_addr = addr;

	// get register snapshot
// TODO rm - don't need to get current registers to figure out address?
//	ptrace(PTRACE_GETREGS, pid, NULL, &regs);
	

	// backup
	long backup = ptrace(PTRACE_PEEKTEXT, pid, bp->code_addr);

	// inject breakpoint
	ptrace(PTRACE_POKETEXT, pid, bp->code_addr, (backup & TRAP_MASK) | TRAP_INST);

        // TODO PTRACE_SETREGS aqui

	bp->code_backup = backup;
	return bp;
}


//*/
                              



static void
inf_trace_me()
{
	// trace from start up
	ptrace( PT_TRACE_ME, 0, NULL, NULL );
}


pid_t
fork_inferior(char** prog, void (*traceme_fun) (void) )
{
	// omitting tty checks here

/*
	char* identifier = NULL;
	if(NULL == (identifier = calloc(IDENTIFIER_SIZE, sizeof(*identifier)))){
		perror("calloc() failed");
		exit(EXIT_FAILURE);
	}
	memset(identifier, '\0', IDENTIFIER_SIZE);
//*/
	pid_t pid=0, pid_parent=getpid();

	if(0 > (pid = fork())){
		perror("fork failed");
		exit(EXIT_FAILURE);

	}else if(pid == 0){
		/* child code */
		child(prog, pid_parent, traceme_fun );

	}else{
		/* parent code */
		tracer(pid);
		
		return pid;

	}
	return pid;
}


static void
inf_ptrace_create_inferior(char** prog) // omitting further params
{

	char chr[16];
// TODO allow for more digits
	do{
		fprintf(stdout, "Set a breakpoint address e.g. '0x0804847c', or [r]un program?\n> ");
		fflush(stdout);
		fflush(stdin);


		memset(chr, '\0', sizeof(chr));
		fgets(chr, sizeof(chr), stdin);  

// TODO so far this is just a dummy
// TODO hex check when providing a breakpoint

//		puts("TODO");
		int idx;
		if (chr[0] == 'r' || chr[0] == 'R') {
			break;
		}else if (chr[0] == '0' && chr[1] == 'x'){
			chr[sizeof(chr)-1] = '\0';
			for (idx=0; idx<sizeof(chr); ++idx) {
// TODO to lower case
// TODO check valid characters
				if ('\n' == chr[idx]) {
					chr[idx] = '\0';
					break;
				}
			}
			printf("TODO - hex number '%s'\n", chr);    

		} else {
			printf( "TODO: help text\n" );
		}
// TODO take care of having remaining tokens in stdin stream.. 

	} while (1);
	
	fprintf(stdout, "\n\nStart program '%s'\n", prog[1]);
	fflush(stdout);


	// ommitting stack handling here
	fork_inferior(prog, inf_trace_me);

// TODO check out number of ntraps: START_INFERIOR_TRAPS_EXPECTED
//	startup_inferior( 7 );


// TODO   
//	inf_ptrace_mourn_inferior(); // TODO params target_ops ptr
}


                                                                               

int
main( int argc, char** argv )
{
	if (argc < 2 || !strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
		fprintf(stderr, "\n");
		fprintf(stderr, "Usage: %s [ -h | --help ]\n", argv[0]);
		fprintf(stderr, "       %s PROGRAM [ ARGS ... ]\n", argv[0]);
		fprintf(stderr, "\n");
		fprintf(stderr, "This program executes PROGRAM in a child process,\n");
		fprintf(stderr, "\n");
		return 1;
	}

	// just some starting point
	inf_ptrace_create_inferior(argv);

	exit( EXIT_SUCCESS );
}
