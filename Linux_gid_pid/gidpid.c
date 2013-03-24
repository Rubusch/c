// gidpid.c
/*
  set a group ID - process ID
  
  #include <sys/types.h>
  #include <unistd.h>

  Avoids orphaned processes when parent terminates. When the parent
  dies this would be a Zombie.

  Thus all subsequent processes should be of a process group due to they 
  are to be terminated when the process group id has to terminate. this ID 
  is the same as that of the process group leader.

  How to set a gid or a pid:
  setpgid() / getpgid()
  setpgrp() / getpgrp()
      set or get a process group ID or group

  setsid() 
      creates a session and sets the process group ID

  getuid() / geteuid()
      get user ID

  setgid() 
      set group ID

  getgid() / getegid()
      get real/effective group ID

  setreuid() / setregid()
      set real user or group ID

  Requires ROOT access!
//*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <unistd.h>
#include <wait.h>

extern pid_t getpgid(pid_t);
extern int setpgrp();

#define CHILD_TXT "\tchild - "
#define PARENT_TXT "parent - "
#define IDENTIFIER_SIZE 10


int main()
{
  pid_t pid=0, pgid=12345, pidParent=getpid();
  char* identifier  = NULL;
  if(NULL == (identifier = calloc(IDENTIFIER_SIZE, sizeof(*identifier)))){
    perror("malloc() failed");
    exit(1);
  }
  memset(identifier, '\0', IDENTIFIER_SIZE);

#ifdef __gnu_linux__
    // requires root access! Generally fails (exec() before?)!?
    // pid - the process to set, if 0, the current process
    // gpid - the new process group id, if 0, the process group id of pid is used
    printf("%s%s = setgpid(pid = %i, gpid = %i);\r\n", identifier, 
	   (setpgid(pidParent, pgid) ? "failed" : "succeeded"), pidParent, pgid);
    
#endif

  // fork()
  printf("fork()\r\n");
  if(0 > (pid = fork())){
    perror("fork() failed");
    exit(1);

  }else if(0 == pid){
    // child code
    strncpy(identifier, CHILD_TXT, IDENTIFIER_SIZE);
    printf("%schild pid %i, parent pid %i\r\n", identifier, getpid(), pidParent);
    sleep(3);
    pgid = getpgid(getpid());
    printf("%sgroup id %i\r\n", identifier, pgid);

    printf("%sdone\r\n", identifier);
    exit(0);

  }else if(0 < pid){    
    // parent code
    strncpy(identifier, PARENT_TXT, IDENTIFIER_SIZE);
    printf("%schild pid %i, parent pid %i\r\n", identifier, pid, getpid());

    if(0 > pgid){
      perror("setting process group id failed");
      _exit(1); // only exit forked process
    }

    int childExitState;
    while(0 == wait(&childExitState))
      ;

    pgid = getpgid(getpid());
    printf("%sgroup id %i\r\n", identifier, pgid);
    
    printf("%sdone\r\n", identifier);
    exit(0);
  }
}
