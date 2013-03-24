// semget.c
/*
  semaphores testing grounds - semget()
//*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define KEY_DIGITS 5
#define NSEMS_DIGITS 4

int isnumber(const char*, const unsigned int);
void readnumber(unsigned int*, const unsigned int, const char*);
void readdigit(unsigned int*, const char*);


int main(int argc, char** argv)
{  
  // key
  key_t key=0;
  readnumber((unsigned int*) &key, KEY_DIGITS, "enter a semaphor key");


  // nsems
  fprintf(stderr, 
	  "nsems - the number of elements in the semaphor array, the call fails\nif nsems is greater than the entered key");
  unsigned int nsems=0;
  readnumber(&nsems, NSEMS_DIGITS, "enter nsems value");


  // sem_flag  
  fprintf(stderr, "expected flags for the sem_flag vlaue:\n");
  fprintf(stderr, "1\tIPC_EXCL (%#8.8o)\n", IPC_EXCL);
  fprintf(stderr, "2\tIPC_CREAT (%#8.8o)\n", IPC_CREAT);
  fprintf(stderr, "3\towner read (%#8.8o)\n", 0400);
  fprintf(stderr, "4\towner alter (%#8.8o)\n", 0200);
  fprintf(stderr, "5\tgroup read (%#8.8o)\n", 040);
  fprintf(stderr, "6\tgroup alter (%#8.8o)\n", 020);
  fprintf(stderr, "7\tother read (%#8.8o)\n", 04);
  fprintf(stderr, "8\tother alter (%#8.8o)\n", 02);
  unsigned int sem_flag=0;
  unsigned int chosen=0;
  do{
    readdigit(&chosen, "select a sem_flag");
    switch(chosen){
    case 1:
      puts("IPC_EXCL");
      sem_flag = IPC_EXCL;
      break;

    case 2:
      puts("IPC_CREAT");
      sem_flag = IPC_CREAT;
      break;

    case 3:
      puts("owner read");
      sem_flag = 0400;
      break;
      
    case 4:
      puts("owner alter");
      sem_flag = 0200;
      break;

    case 5:
      puts("group read");
      sem_flag = 040;
      break;

    case 6:
      puts("group alter");
      sem_flag = 020;
      break;

    case 7:
      puts("other read");
      sem_flag = 04;
      break;
      
    case 8:
      puts("other alter");
      sem_flag = 02;
      break;

    default:
      chosen = 0;
      break;
    }
  }while(!chosen);


  // semget()
  fprintf(stderr, "calling semget(%#lx, %d, %#o)\n", (long unsigned int) key, nsems, sem_flag);
  int sem_id=0;
  if(0 > (sem_id = semget(key, nsems, sem_flag))){
    perror("semget failed");
    exit(EXIT_FAILURE);
  }else{
    fprintf(stderr, "semget returned %d\n", sem_id);
  }

  puts("READY.");
  exit(EXIT_SUCCESS);
}


int isnumber(const char* szNum, const unsigned int szNum_size)
{
  char arr[szNum_size];
  memset(arr, '\0', szNum_size);
  strncpy(arr, szNum, szNum_size);
  arr[szNum_size-1] = '\0';

  const unsigned int len = strlen(arr);
  if(0 == len) return 0;

  int idx;
  for(idx=0; idx < len; ++idx){
    if( ('\n' == arr[idx]) || ('\0' == arr[idx])) break;
    if( !isdigit( arr[idx] )) return 0;
  }
  return 1;
}


void readnumber(unsigned int* iVal, const unsigned int digits, const char* comment)
{
  if(NULL == comment){
    perror("text is NULL");
    return;
  }
  if(NULL == iVal){
    perror("iVal is NULL");
    return;
  }

  unsigned int size = digits+1;
  char cTxt[size];
  do{
    // reset
    memset(cTxt, '\0', size);
    puts(comment);

    // read in
    unsigned int c;
    unsigned int idx=0;

    // in case of exceeding the size of the variable - put a '\0' at the end 
    // and read until '\n', but don't store the characters for cleaning the 
    // stream
    for(idx=0; ('\n' != (c = fgetc(stdin))); ++idx){
      if((size-1) > idx) cTxt[idx] = c;
      else if((size-1) == idx){
	puts("input too long - will be reset");
	memset(cTxt, '\0', size);
	// or cut here:
	//cTxt[idx] = '\0';
      }
    }
  }while(!isnumber(cTxt, (1 + strlen(cTxt))));
  *iVal = atoi(cTxt);
}


void readdigit(unsigned int* iVal, const char* comment)
{
  char cChr[3];
  do{
    // reset
    memset(cChr, '\0', 3);
    puts(comment);

    // read
    fgets(cChr, 3, stdin);
   
    // check stdin and clean in case input exceeded the 
    if('\n' == cChr[1]){
      // ok - do nothing here

    }else if(('\n' == cChr[0]) || ('\n' == cChr[2])){
      // number too long or input empty - erease
      memset(cChr, '\0', 3);

    }else{
      // '\n' still in stdin - clean up
      puts("input too long - will reset");
      memset(cChr, '\0', 3);
      while('\n' != fgetc(stdin));
    }

    // CAUTION: due to '\n' checking the set of '\0' here!
    cChr[2] = '\0';

    // number?
  }while(!isdigit(cChr[0]));

  // transform
  *iVal = atoi(cChr);
}
