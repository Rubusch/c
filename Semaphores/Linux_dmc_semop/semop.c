// semop.c
/*
  semaphore testing grounds - semop()

  struct sembuf{
    unsigned short sem_num; // semaphore index in array
    short sem_op;           // semaphore operation
    short sem_flg;          // operation flag
  };

  semun implementations can vary, 

  union semun{
    int val;                  // value for SETVAL
    struct semid_ds* buf;     // buffer for IPC_STAT & IPC_SET
    unsigned short* array;    // array for GETALL & SETALL
    // optional ?!
    struct seminfo *__buf;    // buffer for IPC_INFO
    void *__pad;
  };
//*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include "lothar/console_input.h"

#define SEM_NUM_DIGITS 5
#define SEM_OP_DIGITS 5
#define SEM_ID_DIGITS 5

static int ask();

static struct semid_ds sem_id_ds;

union semun{
  int val;                  // value for SETVAL
  struct semid_ds* buf;     // buffer for IPC_STAT & IPC_SET
  unsigned short* array;    // array for GETALL & SETALL
  // optional: (?!)
  struct seminfo *__buf;    // buffer for IPC_INFO
  void *__pad;
};


int main(int argc, char** argv)
{
  register unsigned int idx=0;
  int sem_id=0;

  // ptr to operations to perform
  struct sembuf* sem_ops;

  // number of operations still pending
  int num_sem_ops=0;
  do{
    num_sem_ops = ask(&sem_id, &sem_ops);

    for(idx=0; idx<num_sem_ops; ++idx){
      fprintf(stderr, "enter values for operation %d of %d.\n", idx+1, num_sem_ops);
      fprintf(stderr, "sem_num(valid vlaues are 0 <= sem_num < %ld):\n", sem_id_ds.sem_nsems);
      readnumber((unsigned int*) &sem_ops[idx].sem_num, SEM_NUM_DIGITS, "enter a sem_num value:");

      fprintf(stderr, "sem_op\n");
      readnumber((unsigned int*) &sem_ops[idx].sem_op, SEM_OP_DIGITS, "enter a sem_op value:");

      fprintf(stderr, "expected flags in sem_flg are:\n");
      fprintf(stderr, "1\tIPC_NOWAIT (%d)\n", IPC_NOWAIT);
      fprintf(stderr, "2\tSEM_UNDO (%d)\n", SEM_UNDO);
      unsigned int chosen=0;
      do{
	readdigit(&chosen, "select a sem_flg");

	switch(chosen){
	case 1:
	  puts("IPC_NOWAIT");
	  sem_ops->sem_flg = IPC_NOWAIT;
	  break;

	case 2:
	  puts("SEM_UNDO");
	  sem_ops->sem_flg = SEM_UNDO;
	  break;

	default:
	  chosen = 0;
	  break;
	}

      }while(chosen == 0);
    }
    /* recap the call to be made */
    
    fprintf(stderr, "calling semop(%d, &sem_ops, %d)\n", sem_id, num_sem_ops);
    for(idx=0; idx<num_sem_ops; ++idx){
      fprintf(stderr, "sem_ops[%d].sem_num = %d, ", idx, sem_ops[idx].sem_num);
      fprintf(stderr, "sem_op = %d, ", sem_ops[idx].sem_op);
      fprintf(stderr, "sem_flg = %#o)\n", sem_ops[idx].sem_flg);
    }
    
    if(0 > (idx = semop(sem_id, sem_ops, num_sem_ops))){
      perror("semop failed");
      exit(EXIT_FAILURE);
    }else{
      fprintf(stderr, "semop returned %d\n", idx);
    }
  }while(num_sem_ops);

  /* free here */
  if(NULL != sem_ops) free(sem_ops); 
  puts("READY.");
  exit(EXIT_SUCCESS);
}


static int ask(unsigned int* pSem_id, struct sembuf** pSem_ops)
{
  // argument to semctl
  static union semun sem_un;
  
  // work area
  int idx=0;
  
  // size of currently allocated sem_buf array
  static unsigned int num_sem_ops=0;
  
  // sem_id supplied by user
  static unsigned int sem_id = -1;

  // pointer to allocated array
  static struct sembuf *sem_ops = NULL;

  if(sem_id < 0){
    // first call - get sem_id from user and the current state of the semaphore set
    readnumber(&sem_id, SEM_ID_DIGITS, "enter a sem_id value");
    *pSem_id = sem_id;
    
    sem_un.buf = &sem_id_ds;
    if(0 > semctl(sem_id, 0, IPC_STAT, sem_un)){
       perror("semctl failed - IPC_STAT");
    }else{
      if(NULL == (sem_un.array = calloc(sem_id_ds.sem_nsems, sizeof(*sem_un.array)))){
	perror("calloc failed");
	exit(EXIT_FAILURE);
      }
    }
  }
  
  // print current semaphore values
  if(sem_id_ds.sem_nsems){
    fprintf(stderr, "there are %ld semaphores in the set\n", sem_id_ds.sem_nsems);
    if(0 > semctl(sem_id, 0, GETALL, sem_un)){
      perror("semctl failed - GETALL");
    }else{
      fprintf(stderr, "current semaphore values are:");
      for(idx=0; idx < sem_id_ds.sem_nsems; fprintf(stderr, " %d", sem_un.array[++idx]))
	fprintf(stderr, "\n");
    }
  }

  // free here? - in case check!
  if(NULL != sem_un.array) free(sem_un.array);

  // find out how many operations are going to be done in the next call and allocate enough space to do it
  unsigned int tmp_num_sem_ops=0;
  readdigit(&tmp_num_sem_ops, "enter the number of semaphores (one digit, 0 to exit)");
  if(tmp_num_sem_ops > num_sem_ops){
    if(num_sem_ops && (sem_ops != NULL)) free(sem_ops);
    num_sem_ops = tmp_num_sem_ops;
    if(NULL == (sem_ops = calloc(num_sem_ops, sizeof(*sem_ops)))){
      perror("calloc failed");
      exit(1);
    }
  }
  *pSem_ops = sem_ops;

  return tmp_num_sem_ops;
}
