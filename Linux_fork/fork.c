// fork.c
/*
  #include <sys/types.h>
  #include <unistd.h>

  pid_t fork()

  The environment, resouce limits, umask, controlling terminal, 
  current working directory, root directory, signal masks and 
  other process resources are also duplicated from the parent 
  in the forked child process.

  Returns 0 to the child and the pid of the child if everything's ok,
  returns -1 in case of an error.

  pitfalls:
  - intermixed outputs -> use wait() and read-write-lock
  - close() on exit
  - file locks not inherited
  - race conditions -> use wait() [sleep() isn't sufficient?]



  #include <stdlib.h>
  exit(int status);  

  #include <unistd.h>
  void _exit(int status);

  status:
  0  - normal
  >0 - error code

  exit() calls _exit() internally, the kernel system call _exit() 
  will cause the kernel to close descriptors and 
  perform the kernel terminating process clean-up.
  exit() flushes I/O buffers and performs aditional clean-up 
  before _exit().
  Calling exit() allows a parent process to examine a child's process
  error state, without exit() this won't return any value.

  exit() isn't equal to free() for allocated memory and its not a kind of
  garbage collection!
//*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <unistd.h>

#define IDENTIFIER_SIZE 80
#define CHILD_TXT "child process - "
#define PARENT_TXT "parent process - "

int varGlobal = 2;


int main()
{
  int varStack = 20;
  pid_t pid=0, my_pid=getpid(); // pid of PARENT
  char* identifier=NULL;
  if(NULL == (identifier = calloc(IDENTIFIER_SIZE, sizeof(*identifier)))){
    perror("malloc() failed");
    exit(1);
  }
  memset(identifier, '\0', IDENTIFIER_SIZE);

  /*
    START HERE
  //*/

  // try to generate child
  if(0 > (pid = fork())){     
    // fork failed 
    perror("fork() failed");
    exit(1);
    
  }else if(pid == 0){
    // code only executed by CHILD process 
    strncpy(identifier, CHILD_TXT, strlen(CHILD_TXT) + 1);
    ++varGlobal;
    ++varStack;
    my_pid = getpid(); // sets pid of child
    
  }else{    
    // code only executed by PARENT process
    strncpy(identifier, PARENT_TXT, strlen(PARENT_TXT) + 1);
  }

  // code executed by BOTH
  printf("%i %s varGlobal: %i (2), varStack: %i (20)\r\n", my_pid, identifier, varGlobal, varStack);

  free(identifier);
  exit(0);
}
