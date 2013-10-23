// debugger
/*
  @author: Lothar Rubusch
  @email: L.Rubusch@gmx.ch
  @license: EPL
  @2013-october-22


  NOTES

  pid_t  waitpid(pid_t pid, int* status, int options)

  WIFEXITED() - true if child exited due to exit() or _exit()
  WEXITSTSTUS() - contains the exit code in case of exit() or _exit()
  WIFSIGNALED() - true, if child exited due to a signal
  WIFTERMSIG() - contains the signal number in case of a signal
  WIFSTOPPED() - contains if child stopped (signal)


  #include <sys/wait.h>
  pid_t waitpid(pid_t pid, int* status, int options);

  As wait(), but wait's on a particular child process (pid) which doesn't need
  to be the first one.


  PARAMS

  pid - pid of the child process
  status - ???
  options - 3 possibilities:
      WCONTINUED func shall report the status of any continued child process
                 specified by pid whose status has not been reported since it
                 continued from a job control stop

      WNOHANG    func shall not suspend execution of the calling thread of
                 status is not immediately available for the one of the child
                 process pid

      WUNTRACED  status of any child processes specified by pid that are
                 stopped shall be reported to the requested process


  MACROS

  WIFEXITED()    evaluates to a non-zero value if "status" was returned for a
                 child process that terminated normally

  WEXITSTATUS()  if the value is non-zero, this macro evaluates to the low-order
                 8 bits of "status" that the child process passed to _exit() or
                 exit(), or the value the child pid returned from main()

  WIFSIGNALED()  evaluates to a non-zero value of "status"was returned for a
                 child process that terminated due to the receipt of a signal
                 that was not caught

  WTERMSIG()     if WIFSIGNALED is non-zero, this macro evaluates to the number
                 of the signal that caused the termination of the child pid

  WIFSTOPPED()   evaluates the value of WFISTOPPED is non-zero, this macro
                 evaluates to the number of the signal that caused the child
		 process to stop

  WSTOPSIG()     if the value of WIFSTOPPED is non-zero, this macro evaluates
                 to the number of the signal that caused the child process to
		 stop

  ERRORs

  ECHILD         child does not exist or pid is not a child
  EINVAL         invalid "option"
  EINTR          func was interrupted by a signal
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

void child( char* identifier, pid_t pid_parent )
{
	printf("child pid: %i, parent: %i\r\n"
	       , getpid(), pid_parent);

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

	
	// simple exec
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


int main()
{
/*
  // in case reading of settings file
	char envFile[] = "environmental_variables.conf";
	char **listOfEnvVars = NULL;
	int sizeOfEnvVars = 0;
	if(0 != readFile(envFile, &listOfEnvVars, &sizeOfEnvVars)){
		perror("readFile() failed");
		exit(EXIT_FAILURE);
	}
//*/

	char* identifier = NULL;
	if(NULL == (identifier = calloc(IDENTIFIER_SIZE
					, sizeof(*identifier)))){
		perror("calloc() failed");
		exit(EXIT_FAILURE);
	}
	memset(identifier, '\0', IDENTIFIER_SIZE);
	pid_t pid=0, pid_parent=getpid();


	// fork()
	if(0 > (pid = fork())){
		perror("fork failed");
		exit(EXIT_FAILURE);

	}else if(pid == 0){
		// child code
		child( identifier, pid_parent );
	}else{
		// parent code
		strncpy(identifier, PARENT_TXT, strlen(PARENT_TXT));
		printf("%swaiting on pid %i\r\n", identifier, pid);

		

		
		// resurrection testing grounds
		sleep(3);
		kill(pid, SIGTSTP );
		sleep(5);
		kill(pid, SIGCONT );
		

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
		printf("%swaiting done\r\n", identifier);

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
		exit(EXIT_SUCCESS);
	}
	exit(EXIT_SUCCESS);
}
