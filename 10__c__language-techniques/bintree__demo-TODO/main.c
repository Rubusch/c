// main.c
/*
 * @author: Lothar Rubusch
 * @email: L.Rubusch@gmx.ch
 * @license: GPLv3
 *
 * main and CLI to work on trees
//*/
// FIXME: populate inserts 3 instead of 2 elements, when 2 were selected!!!

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lotharlib/console_input.h"
#include "tree/tree.h"
#include "tree_access.h"

/*
  user access

  arguments: print a tree with
  argv[1] = root
  argv[2] = number of elements
//*/
int main(int argc, char **argv)
{
  puts("common tree");

  unsigned int number_of_nodes = 0;
  static leaf *root;
  unsigned int select = 0;

  /*  // debugging input
  init_root(&root, &number_of_nodes, 100);
  add_element(root, &number_of_nodes, 50);
  add_element(root, &number_of_nodes, 150);
  add_element(root, &number_of_nodes, 25);
  //  add_element(root, &number_of_nodes, 75);
  add_element(root, &number_of_nodes, 125);
  add_element(root, &number_of_nodes, 175);
  add_element(root, &number_of_nodes, 20);
  add_element(root, &number_of_nodes, 30);
  //  add_element(root, &number_of_nodes, 70);
  //  add_element(root, &number_of_nodes, 80);
  add_element(root, &number_of_nodes, 120);
  add_element(root, &number_of_nodes, 130);
  add_element(root, &number_of_nodes, 170);
  add_element(root, &number_of_nodes, 180);
  write_tree(root);
  quit(&root, &number_of_nodes);
  exit(EXIT_SUCCESS);
  //*/

  // use of command line arguments
  if (1 < argc) {
    puts("..automated with command line arguments");
    if (3 == argc) {
      // generate root
      fprintf(stderr, "tree %d root generated\n", atoi(argv[1]));
      init_root(&root, &number_of_nodes, atoi(argv[1]));

      // populate
      fprintf(stderr, "tree with %d nodes generated\n", atoi(argv[2]));
      populate_tree(root, &number_of_nodes, atoi(argv[2]));

      // write
      puts("write tree to file..");
      write_tree(root);
      puts("done.");

      // deleteall
      quit(&root, &number_of_nodes);
    } else {
      puts("Ooups, try:");
      fprintf(stderr, "%s <root data> <number of nodes>\n", argv[0]);
      exit(EXIT_FAILURE);
    }
    puts("\n");
    exit(EXIT_SUCCESS);
  }

  // use of looped front end
  do {
    printf("\n\navailable operations:\n");
    printf("0\tquit\n");
    printf("1\tgenerate root\n");
    printf("2\tpopulate\n");
    printf("3\tadd an node\n");
    printf("4\tfind an node\n");
    printf("5\tdelete an node\n");
    printf("6\tdelete all\n");
    printf("7\tshow number of nodes\n");
    printf("8\tprint tree to file\n");
    readdigit(&select, "select an operation:");

    if (0 == select) { // exit
      puts("-> QUIT");
      if (0 < number_of_nodes) {
        quit(&root, &number_of_nodes);
        printf("done - the tree now contains %d elements\n", number_of_nodes);
      } else {
        printf("no tree to delete, aborting anyway..\ndone.\n");
      }
      break;

    } else if (1 == select) { // generate tree
      puts("-> GENERATE ROOT");
      if (number_of_nodes == 0) {
        unsigned int data = 0;
        printf("the value stored as leaf data, can have %d digits at most\n",
               DATA_DIGITS);
        readnumber(&data, DATA_DIGITS, "enter a number:");
        init_root(&root, &number_of_nodes, data);
        printf("%d element (should be 1)\ndone.\n", number_of_nodes);
      } else {
        printf("root already generated: %d\n%d elements\nfailed.\n", root->data,
               number_of_nodes);
      }

    } else if (2 == select) { // populate
      puts("->POPULATE TREE");
      if (0 == number_of_nodes) {
        puts("the tree is empty, first create a tree root\nfailed.");
      } else if (1 <= number_of_nodes) {
        unsigned int number = 0;
        do {
          readnumber(&number, NUM_OF_NODES_DIGITS, "enter a number of nodes");
        } while ((number < 0) || (number > MAX_NODES));
        populate_tree(root, &number_of_nodes, number);
        printf("the tree was populated and now contains %d elements\ndone.\n",
               number_of_nodes);
      } else {
        printf("%d elements\nfailed.\n", number_of_nodes);
      }

    } else if (3 == select) { // insert a node
      puts("-> INSERT ELEMENT");
      if (0 == number_of_nodes) {
        puts("the tree is empty, first create a tree root");
      } else if (MAX_NODES == number_of_nodes) {
        printf("the tree is stuffed, it already contains %d nodes, (max. %d "
               "nodes possible)\n",
               number_of_nodes, MAX_NODES);
      } else {
        puts("enter a data value for the new node");
        unsigned int data = 0;
        readnumber(&data, DATA_DIGITS, "enter data value");
        if (0 > add_element(root, &number_of_nodes, data)) {
          puts("failed.");
        } else {
          printf("%d elements\ndone.\n", number_of_nodes);
        }
      }

    } else if (4 == select) { // find
      puts("-> FIND ELEMENT");
      if (0 == number_of_nodes) {
        puts("the tree is empty, first create a tree root");
      } else {
        unsigned int data = 0;
        readnumber(&data, DATA_DIGITS, "enter data value");
        leaf *element = NULL;
        if (0 > find_element(root, element, data)) {
          printf("not found\n");
        } else {
          printf("found\n");
        }
        printf("%d elements\ndone.\n", number_of_nodes);
      }

    } else if (5 == select) { // delete a leaf
      puts("-> DELETE ELEMENT");
      if (1 >= number_of_nodes) {
        puts("the tree is basically empty, first create a root node and some "
             "leafs to delete");
      } else {
        unsigned int data = 0;
        readnumber(&data, DATA_DIGITS, "enter data value");
        if (0 > delete_element(root, &number_of_nodes, data)) {
          puts("failed.");
        } else {
          printf("%d elements\ndone.\n", number_of_nodes);
        }
      }

    } else if (6 == select) { // delete all
      puts("-> DELETE ALL");
      if (0 == number_of_nodes) {
        puts("the tree is empty, first create one before you like to delete "
             "the entire thing!");
      } else {
        if (0 > delete_tree(&root, &number_of_nodes)) {
          puts("failed.");
        } else {
          printf("%d elements\ndone.\n", number_of_nodes);
        }
      }

    } else if (7 == select) {
      puts("-> NUMBER OF ELEMENTS");
      printf("%d elements currently in the tree\ndone.\n", number_of_nodes);

    } else if (8 == select) {
      puts("-> PRINT TO FILE");
      write_tree(root);
    }
  } while (1);

  if (0 != number_of_nodes) {
    delete_tree(&root, &number_of_nodes);
  }
  puts("READY.");
  exit(EXIT_SUCCESS);
}
