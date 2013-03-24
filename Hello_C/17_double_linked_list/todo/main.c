// main.c
/*
  main file to start the proggy
//*/

#include <stdlib.h>
#include <stdio.h>

#include "lib/consoleinput.h"
#include "dl_list.h"

/*
  just to do it once, TRUE and FALSE like in C++ can 
  be realized simply in setting up a define
//*/
#define TRUE 1

unsigned int strsize = 0;
char *ptr_data;
char *ptr_prev_data;


/*
  clean up and free allocated space
//*/
void cleanup()
{
  if(NULL != ptr_data) free(ptr_data);
  if(NULL != ptr_prev_data) free(ptr_prev_data);
  exit(EXIT_SUCCESS);
}


int main()
{
  unsigned int iSelect = 0;

  printf(" *** Double Linked List ***\n\n");
  do{
    puts("Minimum = 2; Maximum = 32");
    readnumber(&strsize, 2, "Enter the maximum size of the strings stored as \"data\" in the list.");
  }while((strsize <= 1) || (strsize > 32));

  ptr_data = NULL;
  if(NULL == (ptr_data = calloc(strsize, sizeof(*ptr_data)))){
    perror("allocation failed");
    cleanup();
  }

  ptr_prev_data = NULL;
  if(NULL == (ptr_prev_data = calloc(strsize, sizeof(*ptr_prev_data)))){
    perror("allocation failed");
    cleanup();
  }

  do{
    /*
      phases of a while loop, processing input, output or other stuff
    //*/

    // 0. pre phase - resets
    memset(ptr_data, '\0', strsize);    
    memset(ptr_prev_data, '\0', strsize);

    // 1. start phase, e.g. ask
    puts("Listoperations are:");
    puts("\t1\t- create list");
    puts("\t2\t- append element");
    puts("\t3\t- prepend element");
    puts("\t4\t- insert element");
    puts("\t5\t- delete an element");
    puts("\t6\t- delete list");
    puts("\t7\t- quit\n");
    readdigit(&iSelect, "Enter a number: ");

    // 2. process phase, e.g. compute
    if(iSelect == 1){
      puts("create list (1)");      
      readstring(ptr_data, strsize, "Enter some text as data content.");
      if(0 == appendelement(ptr_data, strsize)){
	printlist();
	puts("Ok.");
      }else{
	puts("Failed.");
      }

    }else if(iSelect == 2){
      puts("append element (2)");
      readstring(ptr_data, strsize, "Enter some text as data content.");
      if(0 == appendelement(ptr_data, strsize)){
      	printlist();
	puts("Ok.");
      }else{
	puts("Failed.");
      }

    }else if(iSelect == 3){
      puts("prepend element (3)");
      readstring(ptr_data, strsize, "Enter some text as data content.");
      if(0 == insertelement(NULL, 0, ptr_data, strsize)){
      	printlist();
	puts("Ok.");
      }else{
	puts("Failed.");
      }

    }else if(iSelect == 4){
      puts("insert element (4)");
      readstring(ptr_data, strsize, "Enter some text as data content.");
      readstring(ptr_prev_data, strsize, "After which element to place?");
      if(0 == insertelement(ptr_prev_data, strsize, ptr_data, strsize)){
     	printlist();
	puts("Ok.");
      }else{
	puts("Failed.");
      }

    }else if(iSelect == 5){
      puts("delete an element (5)");
      readstring(ptr_data, strsize, "Enter some text as data content.");
      if(0 == removeelement(ptr_data, strsize)){
     	printlist();
	puts("Ok.");
      }else{
	puts("Failed.");
      }

    }else if(iSelect == 6){
      puts("delete list (6)");
      if(0 == removeall()){
      	printlist();
	puts("Ok.");
      }else{
	puts("Failed.");
      }

    }else if(iSelect == 7){
      puts("quit (7)");
      break;

    }else{
      puts("Syntax error - Try again!");
    }

    // 4. after phase - results
    puts("done.\n");

  }while(TRUE);

  // check and in case remove all - cleaning up, HAS to occur!!
  puts("\nremove all");
  if(0 != removeall()){
    puts("deallocation failed!");
    cleanup();
  }
  printlist();

  puts("\nREADY.");
  cleanup();

  // if we come to this point - something went wrong in the free
  exit(EXIT_FAILURE);
}
