// fork.c
/*
  example input: ls -l

  Example to demonstrate
   - execl()
   - fork()
   - exit()
   - wait()
//*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUF_SIZ 1024
#define LIST_SIZ 64


void parse(char buf[BUF_SIZ], char* buf_list[LIST_SIZ], unsigned int*);
void execute(char**, unsigned int);
void freeAndExit(char**, unsigned int, int);
void readstring(char*, const unsigned int, const char*);


void parse(char buf[BUF_SIZ], char* buf_list[LIST_SIZ], unsigned int* list_size)
{
  if (NULL == list_size) {
    perror("list size is NULL");
    exit(1);
  }

  char *tmp = NULL;
  unsigned cnt_tmp=0;
  unsigned cnt_list=0;
  unsigned cnt=0;

  for (cnt=0; (cnt < BUF_SIZ) && ('\0' != *buf); ++cnt) {
// reset
    if (NULL == tmp) {
      if (NULL == (tmp = calloc(BUF_SIZ, sizeof(*tmp)))) {
        perror("calloc failure");
        exit(1);
      }
      memset(tmp, '\0', BUF_SIZ);
    }

// check character
    if (isalpha(buf[cnt]) || isdigit(buf[cnt]) || ispunct(buf[cnt])) {
      if (BUF_SIZ > cnt_tmp) {
        tmp[cnt_tmp] = buf[cnt];
      } else {
        perror("buffer out of bounds");
        freeAndExit(&tmp, BUF_SIZ, 1);
        exit(1);
      }
      ++cnt_tmp;
    } else {
      if(0 < cnt_tmp){
        ++cnt_tmp;
        tmp[cnt_tmp] = '\0';
        buf_list[cnt_list] = tmp;
        ++cnt_list;

// clear for reset
        tmp = NULL;
        cnt_tmp = 0;

      } else {
// do nothing, just another whitespace, tab or similar
      }
    }
  }
  *list_size = cnt_list;
}


void execute(char** buf_list, unsigned int list_size)
{
  int pid = 0, status = 0;

// fork
  if (0 > (pid = fork())) {
    // failed
    perror("fork failed");
    freeAndExit(buf_list, list_size, 1);

  } else if (0 == pid) {
    // child code
    execvp(*buf_list, buf_list);
/*
  execvp() forks and doesn't return,
  if execvp() is successful,
  the following code never will be executed.

  big problem: the allocated space won't be free'd by the prg!
//*/
    perror(*buf_list);
    freeAndExit(buf_list, list_size, 1);

  } else {
    // parent code
/*
  wait on child
//*/
    while(wait(&status) != pid)
      ;

    freeAndExit(buf_list, list_size, 0);
  }
}


void freeAndExit(char** buf_list, unsigned int size, int error)
{
  unsigned int cnt=0;
  for (cnt=0; cnt<size; ++cnt) {
    if (buf_list[cnt] != NULL) {
      free(buf_list[cnt]);
    }
  }
  exit(error);
}


void readstring(char* cTxt, const unsigned int textSize, const char* comment)
{
  if (NULL == comment) {
    perror("text is NULL");
    return;
  }
  if (NULL == cTxt) {
    perror("iTxt is NULL");
    return;
  }

  do {
    // reset
    memset(cTxt, '\0', textSize);
    puts(comment);

    // read in
    unsigned int c;
    unsigned int idx=0;

    // in case of exceeding the size of the variable - put a '\0' at the end
    // and read until '\n', but don't store the characters for cleaning the
    // stream
    for (idx=0; ('\n' != (c = fgetc(stdin))); ++idx) {
      if ((textSize-1) > idx) {
        cTxt[idx] = c;
      } else if ((textSize-1) == idx) {
        puts("input too long - will be reset");
        memset(cTxt, '\0', textSize);
        // or cut here:
        //cTxt[idx] = '\0';
      }
    }

  } while (0 == strlen(cTxt));
}


// main
int main(int argc, char** argv)
{
  char buf[BUF_SIZ];
  memset(buf, '\0', BUF_SIZ);

  char *buf_list[LIST_SIZ];
  int idx=0;
  for(idx=0; idx < LIST_SIZ; ++idx) {
    buf_list[idx] = NULL;
  }

  unsigned int list_size=0;

  while (1) {
    // get input
    readstring(buf, BUF_SIZ, "command (e.g. \"ls -l\"):");

    // split the string into arguments
    parse(buf, buf_list, &list_size);

    // execute the command
    execute(buf_list, list_size);
  }
}
