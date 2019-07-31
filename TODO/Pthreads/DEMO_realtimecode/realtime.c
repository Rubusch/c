// THIS IS NOT EXECUTIBLE ON LINUX

// realtime.c
/*
  "a real-time process has priority over system-related processes"

  David Marshall:
  This example uses the Solaris real-time extensions to make a single bound 
  thread within a process run in the real-time scheduling class. Using a 
  thread in the real-time class is more desirable than running a whole 
  process in the real-time class, because of the many problems that can arise 
  with a process in a real-time state. For example, it would not be desirable 
  for a process to perform any I/O or large memory operations while in realtime, 
  because a real-time process has priority over system-related processes; if 
  a real-time process requests a page fault, it can starve, waiting for the 
  system to fault in a new page. We can limit this exposure by using threads 
  to execute only the instructions that need to run in realtime.

  - since this proggy worked Solaris libs it's not compilable on Linux directly
  - it should be an exercise to translate it (if possible to Linux).
  - the definite solaris Stuff is commented out
//*/

// doesn't need to be define on Linux
#define _REENTRANT 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <thread.h>
/* replace by
#include <pthread.h>
#include <unistd.h>
//*/

#include <sys/priocntl.h>
#include <sys/rtpriocntl.h>

/* 
   thread prototype 
*/
void *rt_thread(void *);


int main()
{
  /* create the thread that will run in realtime */
  thr_create(NULL, 0, rt_thread, 0, THR_DETACHED, 0);    
  /* replace by
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  pthread_create(0, attr, rt_thread, NULL);
  //*/
  
  /* loop here forever, this thread is the TS scheduling class */
  while (1) {
    printf("MAIN: In time share class... running\n"); 
    sleep(1);
  }
  
  return(0);
}


/*
  This is the routine that is called by the created thread
*/
void *rt_thread(void *arg)
{
  pcinfo_t pcinfo;
  pcparms_t pcparms;
  int i;

  /* let the main thread run for a bit */
  sleep(4);
  
  /* get the class ID for the real-time class */
  strcpy(pcinfo.pc_clname, "RT");
  
  if (priocntl(0, 0, PC_GETCID, (caddr_t)&pcinfo) == -1)
    fprintf(stderr, "getting RT class id\n"), exit(1);
  
  /* set up the real-time parameters */
  pcparms.pc_cid = pcinfo.pc_cid;
  ((rtparms_t *)pcparms.pc_clparms)->rt_pri = 10;
  ((rtparms_t *)pcparms.pc_clparms)->rt_tqnsecs = 0;
  
  /* set an infinite time quantum */
  ((rtparms_t *)pcparms.pc_clparms)->rt_tqsecs = RT_TQINF; 
  
  /* move this thread to the real-time scheduling class */
  if (priocntl(P_LWPID, P_MYID, PC_SETPARMS, (caddr_t)&pcparms) == -1)
    fprintf(stderr, "Setting RT mode\n"), exit(1);
  
  /* simulate some processing */
  for (i=0;i<100000000;i++);
  
  printf("RT_THREAD: NOW EXITING...\n");

  thr_exit((void *)0);
  /* replace by
  pthread_exit(0); 
  //*/
}

