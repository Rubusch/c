// debugger
/*
  @author: Lothar Rubusch
  @email: L.Rubusch@gmx.ch

  @license: EPL
  @2013-october-22
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

/*
void freeMemory(char*** ppList, const unsigned int size)
{
	unsigned int idx = 0;
	for(idx=0; idx < size; ++idx){
		if(NULL != (*ppList)[idx]) free((*ppList)[idx]);
	}
	if(NULL != *ppList) free(*ppList);
}
//*/

/*
void cleanup(){
	perror("cleanup() - so far, do nothing..");
	
	// TODO, dealloc
}
//*/


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


                                                                               

// printing reginsters
void
show_registers(FILE *const out, pid_t pid, const char *const note)
{
	struct user_regs_struct regs;
	long r;

	do {
		r = ptrace(PTRACE_GETREGS, pid, &regs, &regs);
	} while (r == -1L && errno == ESRCH);
	if (r == -1L)
		return;

#if (defined(__x86_64__) || defined(__i386__)) && __WORDSIZE == 64
	if (note && *note) {
		fprintf(out, "Task %d: RIP=0x%016lx, RSP=0x%016lx. %s\n", (int)pid, regs.rip, regs.rsp, note);
	} else {
		fprintf(out, "Task %d: RIP=0x%016lx, RSP=0x%016lx.\n", (int)pid, regs.rip, regs.rsp);
	}
#elif (defined(__x86_64__) || defined(__i386__)) && __WORDSIZE == 32
	if (note && *note) {
		fprintf(out, "Task %d: EIP=0x%08lxx, ESP=0x%08lx. %s\n", (int)pid, regs.eip, regs.esp, note);
	} else {
		fprintf(out, "Task %d: EIP=0x%08lxx, ESP=0x%08lx.\n", (int)pid, regs.eip, regs.esp);
	}
#endif
}

                                                                               


void
child( pid_t pid_parent, void (*traceme_fun) (void) )
{
	printf("child pid: %i, parent: %i\r\n"
	       , getpid(), pid_parent);

	/* call function, in case PTRACE_TRACEME */
	(*traceme_fun) ();

// TODO see if environment needs to be stored

/*
// TODO check if needed
	// signal handler, suspend
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = &suspend_handler;
	sigaction( SIGTSTP, &sa, NULL);

	// signal handler, resume
	struct sigaction sb;
	memset(&sb, 0, sizeof(sb));
	sb.sa_handler = &resume_handler;
	sigaction( SIGCONT, &sb, NULL);
//*/


	// flush output streams
	fflush(stdout); 
	fflush(stderr); 


//*	
// simple execvp
	char* flags[] = { (char*) 0 };
	int execReturn = execvp( "./rabbit.exe", flags );    
/*/
// passing executable as command line string
	execvp(argv[1], argv + 1);
        fprintf(stderr, "%s: %s.\n", argv[1], strerror(errno));
//*/

	// TODO signal listener

/*
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


	// function call to execve() does not return, else failure
	perror("Failure! Child EXEC call returned.");
	printError(execReturn);


//	cleanup();  
	exit(EXIT_FAILURE);
}

// version: breakfast
/*
int
tracer(pid_t pid)
{

// attach
		int status;
		ptrace(PTRACE_ATTACH, pid, NULL, NULL);

//		waitpid( pid, &status, 0 ); // TODO check, should perform the same  
		wait(&status);
		if (WIFEXITED(status)) {
			perror("process ended unexpectedly");
			return -1;
		}

		ptrace(PTRACE_SETOPTIONS, pid, 0, PTRACE_O_TRACEEXIT);


// break at "main"
		tracee_addr_t func_ip = get_main_address(pid); // TODO check if referring to a "method name" works here as address    
//		tracee_addr_t func_ip = (tracee_addr_t) 0x0804847c;   

		breakpoint_t *bp;
		if( NULL == (bp = malloc(sizeof(*bp)))) {
			perror("allocation of bp failed");
			exit(EXIT_FAILURE);
		}
		bp->code_addr = func_ip;
		enable(pid, bp);
		breakpoint_t *func_break = bp;

// loop
		breakpoint_t *last_break = NULL;
		while (iteration_run(pid, last_break)) {

// TODO no wait() here?
// iteration_getip(pid);
			long v = ptrace(PTRACE_PEEKUSER, pid, sizeof(long) * REGISTER_IP);
			tracee_addr_t last_ip = (tracee_addr_t) (v - TRAP_LEN);

			if (last_ip == func_ip) {
#if defined(__x86_64)
				int arg = ptrace(PTRACE_PEEKUSER, pid, sizeof(long) * RDI);
#else
				struct user_regs_struct regs;
				ptrace(PTRACE_GETREGS, pid, NULL, &regs);
				int arg = ptrace(PTRACE_PEEKDATA, pid, sizeof(long) + regs.esp);
#endif
				printf("break at main(%d)\n", arg);
				last_break = func_break;



//				printf("hit ENTER");
//				getchar();
			} else {
				printf("DEBUG: unknown trap at '%p'\n", last_ip);
				last_break = NULL;
			}
		}

                                                                               
		return 0;
}
//*/

// version: signal handling
/*
int
tracer(pid_t pid)
{
		strncpy(identifier, PARENT_TXT, strlen(PARENT_TXT));   
		printf("ptracer: waiting on pid %i\r\n", pid);

// TODO add thread silent (needed?)

		
		// resurrection testing grounds
		sleep(3);
		fprintf(stderr, "%ssuspend child\n", PARENT_TXT);
		kill(pid, SIGTSTP );
		sleep(5);
		fprintf(stderr, "%sresume child\n", PARENT_TXT);
		kill(pid, SIGCONT );
		sleep(5);
		fprintf(stderr, "%sstop child completely\n", PARENT_TXT);
		kill(pid, SIGTERM );
		
		return 0;
}
//*/

// version: threader
int
tracer(pid_t pid)
{
	int status = 0;

	while (1) {
		/* Wait for a child event. */
		if (wait_process(pid, &status)) {
			break;
		}

		/* Exited? */
		if (WIFEXITED(status) || WIFSIGNALED(status)) {
			errno = 0;
			break;
		}

		/* At this point, only stopped events are interesting. */
		if (!WIFSTOPPED(status)) {
			continue;
		}

/*
// tids == pid
		/ * Obtain task IDs. * /
		tids = get_tids(&tid, &tids_max, pid);
		if (!tids) {
			break;
		}
*/

		printf("Process %d has %d tasks,", (int)pid, (int)tids);
		fflush(stdout);

		/* Attach to all tasks. */
		for (t = 0; t < tids; t++) {
			do {
				r = ptrace(PTRACE_ATTACH, tid[t], (void *)0, (void *)0);
			} while (r == -1L && (errno == EBUSY || errno == EFAULT || errno == ESRCH));
			if (r == -1L) {
				const int saved_errno = errno;
				while (t-->0) {
					do {
						r = ptrace(PTRACE_DETACH, tid[t], (void *)0, (void *)0);
					} while (r == -1L && (errno == EBUSY || errno == EFAULT || errno == ESRCH));
/*
  You see, most ptrace commands are only allowed when the task is stopped.
  However, the task is not stopped when it is still completing e.g. a
  single-step command. Thus, using the above loop -- perhaps adding a
  millisecond nanosleep or similar to avoid wasting CPU -- makes sure the
  previous ptrace command has completed (and thus the task stopped) before
  we try to supply the new one.
 */
				}
				tids = 0;
				errno = saved_errno;
				break;
			}
		}
		if (!tids) {
			const int saved_errno = errno;
			if (continue_process(pid, &status)) {
				break;
			}
			printf(" failed to attach (%s).\n", strerror(saved_errno));
			fflush(stdout);
			if (WIFCONTINUED(status)) {
				continue;
			}
			errno = 0;
			break;
		}

		printf(" attached to all.\n\n");
		fflush(stdout);

		/* Dump the registers of each task. */
		for (t = 0; t < tids; t++) {
			show_registers(stdout, tid[t], "");
		}
		printf("\n");
		fflush(stdout);

		for (s = 0; s < SINGLESTEPS; s++) {
			do {
				r = ptrace(PTRACE_SINGLESTEP, tid[tids-1], (void *)0, (void *)0);
			} while (r == -1L && errno == ESRCH);
			if (!r) {
				for (t = 0; t < tids - 1; t++)
					show_registers(stdout, tid[t], "");
				show_registers(stdout, tid[tids-1], "Advanced by one step.");
				printf("\n");
				fflush(stdout);
			} else {
				fprintf(stderr, "Single-step failed: %s.\n", strerror(errno));
				fflush(stderr);
			}
		}

		/* Detach from all tasks. */
		for (t = 0; t < tids; t++) {
			do {
				r = ptrace(PTRACE_DETACH, tid[t], (void *)0, (void *)0);
			} while (r == -1 && (errno == EBUSY || errno == EFAULT || errno == ESRCH));
		}
		tids = 0;
		if (continue_process(pid, &status)) {
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

	return 0;
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
                              



static void
inf_trace_me()
{
	// trace from start up
	ptrace( PT_TRACE_ME, 0, NULL, NULL );

	// stop process
        kill(getpid(), SIGSTOP); // currently, stop for singlestep      
}


pid_t
fork_inferior( void (*traceme_fun) (void) )
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
		child(pid_parent, traceme_fun );

	}else{
		/* parent code */
		tracer(pid);
		
		return pid;

	}
	return pid;
}


void
startup_inferior( int ntraps )
{
// TODO initialization of target_waitstatus struct, based on wait read out syscalls for traces (is that possible?)
	; 
}

void
inf_ptrace_mourn_inferior()
{
	;
// TODO handle params, and check if they are valid
//	waitpid (ptid_get_pid (inferior_ptid), &status, 0);

// TODO generic_mourn_inferior()
}



static void
inf_ptrace_create_inferior( ) // omitting further params
{
	// ommitting stack handling here

	fork_inferior( inf_trace_me );

// TODO check out number of ntraps: START_INFERIOR_TRAPS_EXPECTED
	startup_inferior( 7 );

	inf_ptrace_mourn_inferior(); // TODO params target_ops ptr
}


                                                                               

static void
enable( pid_t pid, breakpoint_t *bp)
{
	long orig = ptrace(PTRACE_PEEKTEXT, pid, bp->code_addr);
	ptrace(PTRACE_POKETEXT, pid, bp->code_addr, (orig & TRAP_MASK) | TRAP_INST);
	bp->code_backup = orig;
}
 

/*
int
iteration_run(pid_t pid, breakpoint_t *bp)
{
	if (bp) {
		ptrace(PTRACE_POKEUSER, pid, sizeof(long)*REGISTER_IP, bp->code_addr);

// dissable
		ptrace(PTRACE_POKETEXT, pid, bp->code_addr, bp->code_backup);
		if (!run(pid, PTRACE_SINGLESTEP)) {
			return 0;
		}
// enable
// TODO replace function by code
		enable(pid, bp);
	}
	return run(pid, PTRACE_CONT);
}

static int
run(pid_t pid, int cmd)
{
	int status, last_sig = 0, event;
	while (1) {
		ptrace(cmd, pid, 0, last_sig);
		waitpid(pid, &status, 0);

		if (WIFEXITED(status))
			return 0;

		if (WIFSTOPPED(status)) {
			last_sig = WSTOPSIG(status);
			if (last_sig == SIGTRAP) {
				event = (status >> 16) & 0xffff;
				return (event == PTRACE_EVENT_EXIT) ? 0 : 1;
			}
		}
	}
}

//*/
int
iteration_run(pid_t pid, breakpoint_t *bp) // TODO declare static
{
	if (bp) {
		ptrace(PTRACE_POKEUSER, pid, sizeof(long)*REGISTER_IP, bp->code_addr);

// dissable
		ptrace(PTRACE_POKETEXT, pid, bp->code_addr, bp->code_backup);
//		if (!run(pid, PTRACE_SINGLESTEP)) {    
		
		// pid, PTRACE_CONT
		int last_sig=0;
		while (1) {
			ptrace(PTRACE_SINGLESTEP, pid, NULL, last_sig);

			/* still alive */
			int status;
			waitpid(pid, &status, 0);
			if (WIFEXITED(status)) {
				return 0;
			}
			if (WIFSTOPPED(status)) {
				last_sig = WSTOPSIG(status);
				if (last_sig == SIGTRAP) {
					int event = (status >> 16) & 0xffff;
//					return (event == PTRACE_EVENT_EXIT) ? 0 : 1;   
					if (event == PTRACE_EVENT_EXIT) {
						return 0;
					}
				}
			}
		}


		
//			return 0;
//		}


// enable
// TODO replace function by code
		enable(pid, bp);
	}

	// pid, PTRACE_CONT
	int last_sig=0;
	while (1) {
		ptrace(PTRACE_CONT, pid, NULL, last_sig);

		/* still alive */
		int status;
		waitpid(pid, &status, 0);
		if (WIFEXITED(status)) {
			return 0;
		}
		if (WIFSTOPPED(status)) {
			last_sig = WSTOPSIG(status);
			if (last_sig == SIGTRAP) {
				int event = (status >> 16) & 0xffff;
				return (event == PTRACE_EVENT_EXIT) ? 0 : 1;
			}
		}
	}
	// we never reach here: stop
	return 0;
}
//*/

tracee_addr_t
get_main_address(pid_t pid)
{
	FILE *fp;
	char filename[30];
	char line[85];
	long addr;
	char str[20];

	sprintf(filename, "/proc/%d/maps", pid);

	if (NULL == (fp = fopen(filename, "r"))) {
		exit(EXIT_FAILURE);
	}
	while (NULL != (fgets(line, 85, fp))) {
//		sscanf(line, "%lx-%*lx %*s %*s %s", &addr, str, str, str, str);
		scanf(line, "%lx-%*lx %*s %*s %s", &addr, str, str, str, str);
		if (0 == (strcmp(str, "00:00"))) { // TODO for empty region, but do for start region "main"
			break;
		}
	}
	fclose(fp);

	printf("XXX addr %lx\n", addr);  
	return (tracee_addr_t) addr;
}

                                                                               

int
main( int argc, char** args )
{
	// just some starting point
	inf_ptrace_create_inferior( );

	exit( EXIT_SUCCESS );
}
