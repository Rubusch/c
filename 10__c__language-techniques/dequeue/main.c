// main.c
/*
 * @author: Lothar Rubusch
 * @email: L.Rubusch@gmx.ch
 * @license: GPLv3
 * @date: 2013-april-28
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <assert.h>

#include "dequeue_item.h"

#define NUMBER_OF_ELEMENTS 10
#define CONTENT_SIZE 10
#define CONTENT_PREFIX "Blahblah"

void output();
void insert(int, char *);


int main(int argc, char **argv)
{
	int res = 0;
//	int cnt = 0;
	item_p item = NULL;
/*, *item_a = NULL, *item_b = NULL;
	content_t *content = NULL;

	char content_idx[3];
	char content_prefix[CONTENT_SIZE]; memset(content_prefix, '\0', CONTENT_SIZE);
// */
	fprintf(stdout, "Single Linked Dequeue\n");

	// TEST: particular
	fprintf(stdout, "TEST: 0 == dequeue__size()");
	res = dequeue__size();
	assert(0 == res);
	fprintf(stdout, " - OK\n");

	fprintf(stdout, "TEST: NULL == dequeue__first()");
	item = dequeue__first();
	assert(NULL == item);
	fprintf(stdout, " - OK\n");

	fprintf(stdout, "TEST: NULL == dequeue__last()");
	item = dequeue__last();
	assert(NULL == item);
	fprintf(stdout, " - OK\n");
/*
		content = malloc(sizeof(content));
		if (NULL == content) {
			perror("malloc() for content failed");
			exit(EXIT_FAILURE);
		}
		dequeue__append(content);
		item_a = dequeue__first();
		assert(NULL != item_a);

		item_b = dequeue__last();
		assert(NULL != item_b);
		assert(item_a == item_b);

		dequeue__remove(item_a);
		free(content);
		item_a = dequeue__first();
		item_b = dequeue__last();
		assert(NULL == item_a);
		assert(NULL == item_b);
		assert(item_a == item_b);
	}


	/ * TEST: bulk

	// preparation bulk
	fprintf(stdout, "preparation\n");
	for (cnt = 0; cnt < NUMBER_OF_ELEMENTS; ++cnt) {
		content = NULL;
		content = malloc(CONTENT_SIZE * sizeof(*content));
		if (NULL == content) {
			perror("malloc() failed");
			exit(EXIT_FAILURE); // cleaning up already performed allocations
		}
		strcpy(content_idx, "00");
		strcpy(content_prefix, CONTENT_PREFIX);
		sprintf(content_idx, "%d", cnt);
		content = strcat(content_prefix, content_idx);
		fprintf(stdout, "\telement #%i added: %s\n", cnt, content);
		dequeue__append(content);
	}


	fprintf(stdout, "TEST dequeue\n\n");


	// dequeue size
	{
		res = dequeue__size();
		assert(NUMBER_OF_ELEMENTS == res);
		assert(res == dequeue__size());
		if (NULL != dequeue__first()) {
			for (cnt = 1, deuque_item_t item = dequeue__first()
				     ; item != dequeue__last()
				     ; item = dequeue__next(item), cnt++)
				;
		}
		assert(res == cnt);

	}


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

  printf("Dequeuesize: %i\n", dequeue__size());

  printf("\nRemove item 5\n\n");
  // TODO obtain pointer to element[5]->content, then free        
  removeItemAt(5);

  // output of the dequeue content
  output();

  printf("\nThe item with the content \"CCC\" is now at positions: %i\n",
         find("CCC", 3));
// */

  printf("READY.\n");
  exit(EXIT_SUCCESS);
};

/*
void output()
{
  printf("The dequeue's content:\n");
  int cnt = 0;
  int max = dequeue__size();
  for (cnt = 0; cnt < max; ++cnt) {
    if (getContentAt(cnt) == NULL)
      printf("NO CONTENT HERE!!\n");
    else
      printf("\telement #%i: \"%s\"\n", cnt, getContentAt(cnt));
  }
  printf("\n");
};

void insert(int pos, char sz[])
{
  printf("\tInsert %s at %i\n", sz, pos);
  insertItemAt(pos, sz);
  printf("\n");
};
// */
