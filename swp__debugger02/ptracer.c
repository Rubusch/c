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
	long ins;

	do {
		ins = ptrace(PTRACE_GETREGS, pid, &regs, &regs);
	} while (ins == -1L && errno == ESRCH);
	if (ins == -1L) {
		return;
	}

#if (defined(__x86_64__) || defined(__i386__)) && __WORDSIZE == 64
	ins = ptrace(PTRACE_PEEKTEXT, pid, regs.rip, NULL);
	fprintf(out, "instr: 0x%08lx\n", ins);
	fprintf(out, "PID %d: EIP=0x%08lxx, ESP=0x%08lx ", (int)pid, regs.rip, regs.rsp);
/*
// TODO rm
	if (note && *note) {
		fprintf(out, "PID %d: RIP=0x%016lx, RSP=0x%016lx. %s\n", (int)pid, regs.rip, regs.rsp, note);
	} else {
		fprintf(out, "PID %d: RIP=0x%016lx, RSP=0x%016lx.\n", (int)pid, regs.rip, regs.rsp);
	}
//*/
#elif (defined(__x86_64__) || defined(__i386__)) && __WORDSIZE == 32
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


	// stop child
	kill(getpid(), SIGSTOP);   

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
	long ret;
	char chr = 's';

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

		// set breakpoints
		tracee_addr_t break_addr = 0x0804847c;
		breakpoint_t *bp = set_breakpoint(pid, break_addr);
		ptrace(PTRACE_CONT, pid, NULL, NULL);

			// inject breakpoints
//			if (bp) {
//				ptrace(PTRACE_POKEUSER, pid, sizeof(long)*REGISTER_IP, bp->code_addr);
//			}

			// restore code
//			ptrace(PTRACE_POKETEXT, pid, bp->addr, bp->orig_code);

		// continue





		
		// attach to all tasks - alternatively use PTRACE_TRACEME in the child
/*
		do {
			ret = ptrace(PTRACE_ATTACH, pid, NULL, NULL);   
		} while (ret == -1L && (errno == EBUSY || errno == EFAULT || errno == ESRCH));
		if (ret == -1L) {
			const int saved_errno = errno;

			do {
				ret = ptrace(PTRACE_DETACH, pid, NULL, NULL);   
			} while (ret == -1L && (errno == EBUSY || errno == EFAULT || errno == ESRCH));
			errno = saved_errno;
			break;
		}
//*/



		while (1){
                        // user input
			fprintf( stdout, "Process stopped, hit ENTER for next step");
			fflush(stdout);
			if( 'c' == (chr = (char) getchar())){
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


//*
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

void
get_data(pid_t child, long addr, char *str, int len)
{
	char *laddr;
	int i, j;
	union u{
		long val;
		char chars[sizeof(long)];
	}data;
	i = 0;
	j = len / sizeof(long);
	laddr=str;
	while (i < j) {
		data.val = ptrace(PTRACE_PEEKDATA, child, addr+i*4, NULL);
		memcpy(laddr, data.chars, sizeof(long));
		++i;
		laddr += sizeof(long);
	}
	j = len % sizeof(long);
	/* since long size will be 4, we always will fall into this condition for reading */
	if (j != 0) {
		data.val = ptrace(PTRACE_PEEKDATA, child, addr+i*4, NULL);
		memcpy(laddr, data.chars, j);
	}
	str[len]='\0';
}


void
put_data(pid_t child, long addr, char *str, int len)
{
	char *laddr;
	int i=0, j=len/sizeof(long);
	union u{
		long val;
		char chars[sizeof(long)];
	} data;
	laddr = str;
	while (i < j) {
		memcpy(data.chars, laddr, sizeof(long));
		ptrace(PTRACE_POKEDATA, child, addr+i*4, data.val);
		++i;
		laddr += sizeof(long);
	}
	j = len % sizeof(long);

	/* since long size will be 4, we always will fall into this condition for writing back */
	if (j != 0) {
		memcpy(data.chars, laddr, j);
		ptrace(PTRACE_POKEDATA, child, addr+i*4, data.val);
	}
}
   

breakpoint_t
*set_breakpoint(pid_t pid, tracee_addr_t addr) {
	breakpoint_t *bp = malloc(sizeof(*bp));
	bp->code_addr = addr;

	// get register snapshot
// TODO rm - don't need to get current registers to figure out address
//	ptrace(PTRACE_GETREGS, pid, NULL, &regs);
	

	// backup
	long backup = ptrace(PTRACE_PEEKTEXT, pid, bp->code_addr);

	// inject breakpoint
	ptrace(PTRACE_POKETEXT, pid, bp->code_addr, (backup & TRAP_MASK) | TRAP_INST);

	bp->code_backup = backup;
	return bp;
}


//*/
                              



static void
inf_trace_me()
{
	// trace from start up
	ptrace( PT_TRACE_ME, 0, NULL, NULL );

	// stop process
        kill(getpid(), SIGSTOP); // currently, stop for singlestep      
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
inf_ptrace_create_inferior(char** prog) // omitting further params
{
	// ommitting stack handling here

	fork_inferior(prog, inf_trace_me);

// TODO check out number of ntraps: START_INFERIOR_TRAPS_EXPECTED
	startup_inferior( 7 );

	inf_ptrace_mourn_inferior(); // TODO params target_ops ptr
}


                                                                               
/*
// TODO used for scenario 2: breakfast

static void
enable( pid_t pid, breakpoint_t *bp)
{
	long orig = ptrace(PTRACE_PEEKTEXT, pid, bp->code_addr);
	ptrace(PTRACE_POKETEXT, pid, bp->code_addr, (orig & TRAP_MASK) | TRAP_INST);
	bp->code_backup = orig;
}

int
iteration_run(pid_t pid, breakpoint_t *bp) // TODO declare static
{
	if (bp) {
		ptrace(PTRACE_POKEUSER, pid, sizeof(long)*REGISTER_IP, bp->code_addr);

// dissable
		ptrace(PTRACE_POKETEXT, pid, bp->code_addr, bp->code_backup);

		// pid, PTRACE_CONT
		int last_sig=0;
		while (1) {
			ptrace(PTRACE_SINGLESTEP, pid, NULL, last_sig);

			// still alive
			int status;
			waitpid(pid, &status, 0);
			if (WIFEXITED(status)) {
				return 0;
			}
			if (WIFSTOPPED(status)) {
				last_sig = WSTOPSIG(status);
				if (last_sig == SIGTRAP) {
					int event = (status >> 16) & 0xffff;
					if (event == PTRACE_EVENT_EXIT) {
						return 0;
					}
				}
			}
		}

// enable
// TODO replace function by code
		enable(pid, bp);
	}

	// pid, PTRACE_CONT
	int last_sig=0;
	while (1) {
		ptrace(PTRACE_CONT, pid, NULL, last_sig);

		// still alive
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
//*/
                                                                               

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
