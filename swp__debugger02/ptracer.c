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


void suspend_handler(int signum)
{
	puts("child: suspend_handler called");
}

void resume_handler(int signum)
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

	
	// simple execvp
	char* flags[] = { (char*) 0 };
	int execReturn = execvp( "./rabbit.exe", flags );    
	



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
inf_trace_me(){
        kill(getpid(), SIGSTOP); // currently, stop for singlestep      

//	ptrace( PT_TRACE_ME, 0, (PTRACE_TYPE_ARG3)0, 0 );
// TODO figure out correct values for macros
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
			break;
		}

		ptrace(PTRACE_SETOPTIONS, pid, 0, PTRACE_O_TRACEEXIT);
		

// break at "main"

// TODO check if referring to a "method name" works here as address
		tracee_addr_t addr = main;   

		if( NULL == (breakfast_t *bp = malloc(sizeof(*bp)))) {
			perror("allocation of bp failed");
			exit(EXIT_FAILURE);
		}

		bp->code_addr = addr;

		enable(pid, bp);

		breakpoint_t *fact_break = bp;
		


		while () {
			
		}



                                                                               
//		strncpy(identifier, PARENT_TXT, strlen(PARENT_TXT));   
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
	long orig = ptrace(PTRACE_PEEKTEXT, pid, bp->addr);
	ptrace(PTRACE_POKETEXT, pid, bp->addr, (orig & TRAP_MASK) | TRAP_INST);
	bp->orig_code = orig;
}


                                                                               

int
main( int argc, char** args )
{
	// just some starting point
	inf_ptrace_create_inferior( );

	exit( EXIT_SUCCESS );
}
