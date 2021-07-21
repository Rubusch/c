// main.c
/*
  Implementation of a single linked queue
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue_item.h"

#define NUMBER_OF_ELEMENTS 10

void output();
void insert(int, char *);


int main(int argc, char **argv)
{
  printf("Single Linked Queue\n\n");

  printf("Generate queue:\n");
  char str[] = "Blahblahblah";

  int cnt = 0;
  for (cnt = 0; cnt < NUMBER_OF_ELEMENTS; ++cnt) {
    printf("\telement #%i added: %s\n", cnt, str);
    addItem(str);
  }

  // queue size
  printf("\nQueuesize: %i\n\n", queueSize());

  // output of the queue content
  output();

  char sz[] = "AAA";
  insert(0, sz);
  strcpy(sz, "BBB");
  insert(5, sz);
  strcpy(sz, "CCC");
  insert(NUMBER_OF_ELEMENTS, sz);
  strcpy(sz, "DDD");
  insert(100, sz);

  // output of the queue content
  output();

  printf("Queuesize: %i\n", queueSize());

  printf("\nRemove item 5\n\n");
  removeItemAt(5);

  // output of the queue content
  output();

  printf("\nThe item with the content \"CCC\" is now at positions: %i\n",
         find("CCC", 3));

  printf("READY.\n");
  return 0;
};


void output()
{
  printf("The queue's content:\n");
  int cnt = 0;
  for (cnt = 0; cnt < queueSize(); ++cnt) {
    if (getContent(cnt) == NULL)
      printf("NO CONTENT HERE!!\n");
    printf("\telement #%i: %s\n", cnt, getContent(cnt));
  }
  printf("\n");
};

void insert(int pos, char sz[])
{
  printf("\tInsert %s at %i\n", sz, pos);
  insertItemAt(pos, sz);
  printf("\n");
};
