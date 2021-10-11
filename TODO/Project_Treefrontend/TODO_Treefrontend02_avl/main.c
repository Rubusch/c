// main.c
/*
  main and CLI to work on trees
//*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tools.h"
#include "tree_control.h"

void select_quit(unsigned int *number_of_nodes)
{
	if (0 < *number_of_nodes) {
		quit(number_of_nodes);
		printf("done - the tree now contains %d elements\n",
		       *number_of_nodes);
	} else {
		printf("no tree to delete, aborting anyway..\ndone.\n");
	}
}

void select_generate_root(unsigned int *number_of_nodes)
{
	if (0 == *number_of_nodes) {
		init_root(number_of_nodes);
		printf("%d element (should be 1)\ndone.\n", *number_of_nodes);
	} else {
		printf("root already generated: %d elements\nfailed.\n",
		       *number_of_nodes);
	}
}

void select_insert(unsigned int *number_of_nodes)
{
	unsigned int select = 0;
	do {
		printf("\n\navailable insert options are:\n");
		printf("0\tcancel\n");
		printf("1\tpopulate\n");
		printf("2\tinsert an element\n");
		readdigit(&select, "select an operation:");

		if (1 == select) {
			if (0 == *number_of_nodes) {
				puts("the tree is empty, first create a tree root\nfailed.");
			} else if (1 <= *number_of_nodes) {
				unsigned int number = 0;
				do {
					readnumber(&number, NUM_OF_NODES_DIGITS,
						   "enter a number of nodes");
				} while ((number < 0) || (number > MAX_NODES));
				populate_tree(number_of_nodes, number);
				printf("the tree was populated and now contains %d elements\ndone.\n",
				       *number_of_nodes);
			} else {
				printf("%d elements\nfailed.\n",
				       *number_of_nodes);
			}
			break;

		} else if (2 == select) {
			if (0 == *number_of_nodes) {
				puts("the tree is empty, first create a tree root");
			} else if (MAX_NODES == *number_of_nodes) {
				printf("the tree is stuffed, it already contains %d nodes, (max. %d "
				       "nodes possible)\n",
				       *number_of_nodes, MAX_NODES);
			} else {
				puts("enter a data value for the new node");
				if (0 > add_element(number_of_nodes)) {
					puts("failed.");
				} else {
					printf("%d elements\ndone.\n",
					       *number_of_nodes);
				}
			}
			break;
		}
	} while (0 != select);
}

void select_delete(unsigned int *number_of_nodes)
{
	if (0 == *number_of_nodes) {
		puts("the tree is basically empty, first create a root node and some leafs "
		     "to delete");
		return;
	}

	unsigned int select = 0;
	do {
		printf("\n\navailable delete operations:\n");
		printf("0\tcancel\n");
		printf("1\tdelete a leaf\n");
		printf("2\tdelete a tree part\n");
		printf("3\tdelete a node\n");
		readdigit(&select, "select an operation:");
		if (1 == select) {
			if (0 == *number_of_nodes) {
				puts("the tree is empty, first create one before you like to delete "
				     "the entire thing!");
			} else {
				if (0 > delete_tree(number_of_nodes)) {
					puts("failed.");
				} else {
					printf("%d elements\ndone.\n",
					       *number_of_nodes);
				}
			}
			break;

		} else if (2 == select) {
			if (0 > delete_leaf(number_of_nodes)) {
				puts("failed.");
			} else {
				printf("deleted.\n%d elements\ndone.\n",
				       *number_of_nodes);
			}
			break;

		} else if (3 == select) {
			if (0 > delete_element(number_of_nodes)) {
				puts("failed.");
			} else {
				printf("deleted.\n%d elements\ndone.\n",
				       *number_of_nodes);
			}
			break;
		}

	} while (0 != select);
}

void select_find(unsigned int *number_of_nodes)
{
	if (0 == *number_of_nodes) {
		puts("the tree is empty, first create a tree root");
	} else {
		if (0 > find_element()) {
			printf("not found\n");
		} else {
			printf("found\n");
		}
		printf("%d elements\ndone.\n", *number_of_nodes);
	}
}

void select_copy(unsigned int *number_of_nodes)
{
	unsigned int select = 0;
	do {
		printf("\n\navailable copy operations:\n");
		printf("0\tcancel\n");
		printf("1\tcopy entire tree\n");
		printf("2\tcopy element from tree A to tree B\n");
		readdigit(&select, "select an operation:");
		if (1 == select) {
			puts("TODO - not implemented"); // TODO
			//      int backup_tree(unsigned int*);
			break;

		} else if (2 == select) {
			puts("TODO - not implemented"); // TODO
			//      int backup_element(unsigned int*);
			break;
		}
	} while (0 != select);
}

void select_traverse(unsigned int *number_of_nodes)
{
	unsigned int select = 0;

	do {
		printf("\n\navailable traverse operations:\n");
		printf("0\tcancel\n");
		printf("1\ttraverse pre-order\n");
		printf("2\ttraverse post-order\n");
		printf("3\ttraverse in-order\n");
		readdigit(&select, "select an operation:");
		if (1 == select) {
			//      int traverse_pre_order(unsigned int*);
			break;

		} else if (2 == select) {
			//      int traverse_post_order(unsigned int*);
			break;

		} else if (3 == select) {
			//      int traverse_in_order(unsigned int*);
			break;
		}
	} while (0 != select);
}

void select_rotate(unsigned int *number_of_nodes)
{
	unsigned int select = 0;
	do {
		printf("\n\navailable rotate operations\n");
		printf("0\tcancel\n");
		printf("1\trotate left\n");
		printf("2\trotate right\n");
		readdigit(&select, "select an operation:");
		if (1 == select) {
			//      int rotate_left(unsigned int*);
			break;

		} else if (2 == select) {
			//      int rotate_right(unsigned int*);
			break;
		}
	} while (0 != select);
}

void select_number_of_nodes(unsigned int *number_of_nodes)
{
	printf("%d elements currently in the tree\ndone.\n", *number_of_nodes);
}

void select_print(unsigned int *number_of_nodes)
{
	if (0 < *number_of_nodes) {
		write_tree();
	} else {
		puts("failed - tree empty!");
		return;
	}
	puts("done.");
}

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
	/*
  if(1 < argc){
    puts("..automated with command line arguments");
    if(3 == argc){
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
    }else{
      puts("Ooups, try:");
      fprintf(stderr, "%s <root data> <number of nodes>\n", argv[0]);
      exit(EXIT_FAILURE);
    }
    puts("\n");
    exit(EXIT_SUCCESS);
  }
  //*/

	// use of looped front end
	do {
		printf("\n\navailable operations:\n");
		printf("0\tquit\n");
		printf("1\tgenerate root\n");
		printf("2\tinsert\n");
		printf("3\tdelete\n");
		printf("4\tfind\n");
		printf("5\tcopy\n");
		printf("6\ttraverse\n");
		printf("7\trotate\n");
		printf("8\tshow number of nodes\n");
		printf("9\tprint tree to file\n");
		readdigit(&select, "select an operation:");

		if (0 == select) { // exit
			puts("-> QUIT");
			select_quit(&number_of_nodes);
			break;

		} else if (1 == select) { // generate root
			puts("-> GENERATE ROOT");
			select_generate_root(&number_of_nodes);

		} else if (2 == select) { // insert a node
			puts("-> INSERT");
			select_insert(&number_of_nodes);

		} else if (3 == select) { // delete
			puts("-> DELETE");
			select_delete(&number_of_nodes);

		} else if (4 == select) { // find
			puts("-> FIND ELEMENT");
			select_find(&number_of_nodes);

		} else if (5 == select) { // copy
			puts("-> COPY");
			select_copy(&number_of_nodes);

		} else if (6 == select) { // traverse
			puts("-> TRAVERSE");
			select_traverse(&number_of_nodes);

		} else if (7 == select) { // rotate
			puts("-> ROTATE");
			select_rotate(&number_of_nodes);

		} else if (8 == select) { // number of elements
			puts("-> NUMBER OF ELEMENTS");
			select_number_of_nodes(&number_of_nodes);

		} else if (9 == select) { // print to file
			puts("-> PRINT TO FILE");
			select_print(&number_of_nodes);
		}
	} while (1);

	if (0 != number_of_nodes) {
		delete_tree(&number_of_nodes);
	}
	puts("READY.");
	exit(EXIT_SUCCESS);
}
