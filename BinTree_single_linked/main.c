// main.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bintree_item.h"

#define ELEMENT_COUNT 10

//#define DEBUG


extern void outputTree();
void outputArray(char[][CONTENT_LENGTH]);


int main(int argc, char**argv)
{
  printf("Binary Tree - Single Linked\n\n");

  // generate strings
  printf("Generate strings\n\n");
  char str[ELEMENT_COUNT][CONTENT_LENGTH] = {
    "All", "Alle", "and", "dull", "play", "makes", "Jack", "a", "dull", "boy." };

#ifdef DEBUG
  outputArray( str);
#endif

  // generate tree root
  printf("Generate tree root\n\n");
  generateTree(str[0]);

  // append strings
  printf("Append strings\n\n");
  {
    int cnt=0;
    for(cnt=1; cnt<ELEMENT_COUNT; ++cnt){
      addItemSorted(str[cnt]);
    }
  }

  puts("READY.");
  return 0;
}


void outputArray(char str[][CONTENT_LENGTH])
{
  int i=0;
  for(i=0; i < ELEMENT_COUNT; ++i)
    printf("\t%i.: %s\n", i, str[i]);

  printf("\n");
}


