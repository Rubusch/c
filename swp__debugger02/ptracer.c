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

#include "ptracer.h"


// TODO shift to header     
/* declarations */
struct breakpoint_t;

void printError(int errnum);
void suspend_handler(int signum); 
void resume_handler(int signum); 
void child( pid_t pid_parent, void (*traceme_fun) (void) );
static void inf_trace_me();
pid_t fork_inferior( void (*traceme_fun) (void) );
void startup_inferior( int ntraps );
void inf_ptrace_mourn_inferior();
static void inf_ptrace_create_inferior( );
static void enable( pid_t pid, breakpoint_t *bp);
int iteration_run(pid_t pid, breakpoint_t *bp);
static int run(pid_t pid, int cmd);
tracee_addr_t get_main_address(pid_t pid);


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
		child( pid_parent, traceme_fun );

	}else{
		/* parent code */
// attach
		int status;

		ptrace(PTRACE_ATTACH, pid, NULL, NULL);

		/* child still alive */
//		waitpid( pid, &status, 0 ); // TODO check, should perform the same  
		wait(&status);
		if (WIFEXITED(status)) {
			perror("process ended unexpectedly");
			return -1;
		}

		ptrace(PTRACE_SETOPTIONS, pid, 0, PTRACE_O_TRACEEXIT);


// break at "main"
		tracee_addr_t func_ip = get_main_address(pid); // TODO check if referring to a "method name" works here as address    
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
				struct user_regs_struct s; // TODO rn regs
				ptrace(PTRACE_GETREGS, pid, NULL, &s);
				int arg = ptrace(PTRACE_PEEKDATA, pid, sizeof(long) + s.esp);
#endif
				printf("break at main(%d)\n", arg);
				last_break = func_break;
			} else {
				printf("unknown trap at %p\n", last_ip);
				last_break = NULL;
			}
		}

                                                                               
/*		strncpy(identifier, PARENT_TXT, strlen(PARENT_TXT));   
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
		
//*/
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
 
// TODO
//static int run(pid_t pid, int cmd);
// TODO
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

// TODO
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
	return (tracee_addr_t) addr;
}

                                                                               

int
main( int argc, char** args )
{
	// just some starting point
	inf_ptrace_create_inferior( );

	exit( EXIT_SUCCESS );
}
