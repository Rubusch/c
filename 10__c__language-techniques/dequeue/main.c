// main.c
/*
 * @author: Lothar Rubusch
 * @email: L.Rubusch@gmx.ch
 * @license: GPLv3
 * @date: 2013-april-28
 */
/*
  Implementation of a single linked dequeue
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dequeue_item.h"

#define NUMBER_OF_ELEMENTS 10

void output();
void insert(int, char*);


int main(int argc, char** argv){
  printf("Single Linked Dequeue\n\n");

  char str[] = "Blahblah";

  printf("Generate dequeue:\n");
  int cnt = 0;
  char szInt[3];
  char szStr[8];
  char* szTemp;
  for(cnt=0; cnt<NUMBER_OF_ELEMENTS; ++cnt){
    szTemp = 0;
    strcpy( szInt, "00");
    strcpy( szStr, str);

    szStr[7] = '\0'; // avoids buffer overflow reading - stop token needs to be reset!

    // convert int -> string
    sprintf(szInt, "%3d", cnt);
    szTemp = strcat(szStr, szInt);

    printf("\telement #%i added: %s\n", cnt, szTemp);
    addItem(szTemp); 
  }

  // dequeue size
  printf("\nDequeuesize: %i\n\n", dequeueSize());

  // output of the dequeue content
  output();

  char sz[] = "AAA";
  insert(0, sz);
  strcpy(sz, "BBB");
  insert(5, sz);
  strcpy(sz, "CCC");
  insert(NUMBER_OF_ELEMENTS, sz);
  strcpy(sz, "DDD");
  insert(100, sz);

  // output of the dequeue content
  output();

  printf("Dequeuesize: %i\n", dequeueSize());

  printf("\nRemove item 5\n\n");
  removeItemAt(5);

  // output of the dequeue content
  output();

  printf("\nThe item with the content \"CCC\" is now at positions: %i\n", find("CCC", 3));

  printf("READY.\n");
  return 0;
};


void output()
{
  printf("The dequeue's content:\n");
  int cnt = 0;
  int max = dequeueSize();
  for(cnt=0; cnt < max; ++cnt){
    if(getContentAt(cnt) == NULL) printf("NO CONTENT HERE!!\n");
    else printf("\telement #%i: \"%s\"\n", cnt, getContentAt(cnt));
  }
  printf("\n");
};

void insert(int pos, char sz[])
{
  printf("\tInsert %s at %i\n", sz, pos);
  insertItemAt(pos, sz);
  printf("\n");
};
