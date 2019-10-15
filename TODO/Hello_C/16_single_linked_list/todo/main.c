// main.c
/*
  main file to start the proggy

  A single linked list is easy to implement, but it is quite
  limited. Operations like removing within the list or inserting
  are quite complicated, because they need the previous element
  as pointer.


  Deleting elments "downwards" - decrementing the index

  in some cases e.g. deleting elements from the last one,
  it makes more sense counting backwards.
  In this case you have to check for:

    size != length

    and

    0 <= cnt

output:


creating a list
content: january

appending elements
content: january  february

appending elements
content: january  february  may

inserting elements
content: january  february  april  may

inserting elements
content: january  february  march  april  may

print list content
content: january  february  march  april  may

remove all
content:

READY.
//*/

#include <stdio.h>
#include <stdlib.h>

#include "sl_list.h"


int main()
{
  // create
  puts("\ncreating a list");
  do {
    // append and create automatically the list
    if (0 != appendelement("january", 1 + strlen("january"))) {
      exit(EXIT_FAILURE);
    }
    printlist();

    // append
    puts("\nappending elements");
    if (0 != appendelement("february", 1 + strlen("february"))) {
      break;
    }
    printlist();
    puts("\nappending elements");
    if (0 != appendelement("may", 1 + strlen("may"))) {
      break;
    }
    printlist();

    // insert
    puts("\ninserting elements");
    if (0 != insertelement("february", 1 + strlen("february"), "april",
                           1 + strlen("april"))) {
      break;
    }
    printlist();
    puts("\ninserting elements");
    if (0 != insertelement("february", 1 + strlen("february"), "march",
                           1 + strlen("march"))) {
      break;
    }
    printlist();

    // print
    puts("\nprint list content");
    printlist();
  } while (0);

  // remove all - cleaning up, HAS to occur!!
  puts("\nremove all");
  if (0 != removeall()) {
    puts("deallocation failed!");
    exit(EXIT_FAILURE);
  }
  printlist();

  puts("\nREADY.");
  exit(EXIT_SUCCESS);
}
