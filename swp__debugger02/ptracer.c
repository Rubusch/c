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

#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>


#define PARENT_TXT "parent - "
#define CHILD_TXT "\tchild - "
#define IDENTIFIER_SIZE 20



void printError(int errnum)
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


void freeMemory(char*** ppList, const unsigned int size)
{
	unsigned int idx = 0;
	for(idx=0; idx < size; ++idx){
		if(NULL != (*ppList)[idx]) free((*ppList)[idx]);
	}
	if(NULL != *ppList) free(*ppList);
}


void cleanup(){
	perror("cleanup() - so far, do nothing..");
	
	// TODO, dealloc
}


void suspend_handler(int signum)
{
	puts("child: suspend_handler called");
}

void resume_handler(int signum)
{
	puts("child: resume_handler");
}

void
child( char* identifier, pid_t pid_parent
       , void (*traceme_fun) (void) )
{
	printf("child pid: %i, parent: %i\r\n"
	       , getpid(), pid_parent);

// TODO tty setup, omitted

	// call function
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
// TODO is execvp in fact used?
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
	;  
//	ptrace( PT_TRACE_ME, 0, (PTRACE_TYPE_ARG3)0, 0 );
// TODO figure out correct values for macros
}


pid_t
fork_inferior( void (*traceme_fun) (void) )
{
	// omitting tty checks here


	char* identifier = NULL;
	if(NULL == (identifier = calloc(IDENTIFIER_SIZE
					, sizeof(*identifier)))){
		perror("calloc() failed");
		exit(EXIT_FAILURE);
	}
	memset(identifier, '\0', IDENTIFIER_SIZE);
	pid_t pid=0, pid_parent=getpid();


	// fork()
	if(0 > (pid = fork())){ // TODO in case checkout vfork(), but actually deprecated
		perror("fork failed");
		exit(EXIT_FAILURE);

	}else if(pid == 0){
		// child code
		child( identifier, pid_parent, inf_trace_me ); // terminates

	}else{
		// parent code
		strncpy(identifier, PARENT_TXT, strlen(PARENT_TXT));
		printf("%swaiting on pid %i\r\n", identifier, pid);

// TODO new tty postfork

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
		


		/*
		  wait on error state to return
		  1. set wait condition
		*/
		pid_t pid_wait=-1;

		/*
		  2. child exit status
		*/
		int childExitStatus=0;

		/*
		  3. wait on child, pid_wait = -1, wait on all childs
		*/
		pid_wait = pid;
		while(0 == waitpid( pid_wait, &childExitStatus, 0))
			;
		fprintf(stderr, "%swaiting done\r\n", identifier);

		/*
		  in case: 4. evaluation of term conditions of the child
		*/
		if( WIFEXITED(childExitStatus)){
			printf("%schild exited with exit(): %i\r\n"
			       , identifier
			       , WEXITSTATUS(childExitStatus));
		}

		if( WIFSIGNALED(childExitStatus)){
			printf("%schild exited with a SIGNAL: %i\r\n"
			       , identifier
			       , WIFSIGNALED(childExitStatus));
		}

		if( WTERMSIG(childExitStatus)){
			printf("%schild exited with SIGTERM: %i\r\n"
			       , identifier
			       , WTERMSIG(childExitStatus));
		}

		if( WIFSTOPPED(childExitStatus)){
			printf("%schild exited with SIGSTOP: %i\r\n"
			       , identifier
			       , WIFSTOPPED(childExitStatus));
		}

		printf("%sdone\r\n", identifier);
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


int
main( int argc, char** args )
{
	// just some starting point
	inf_ptrace_create_inferior( );

	exit( EXIT_SUCCESS );
}
