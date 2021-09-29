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

#include "content.h"
#include "dequeue_item.h"

#define NUMBER_OF_ELEMENTS 100
#define STR_SIZE 32
#define STR_IDX_SIZE 3
#define STR_PREFIX_SIZE 10
#define STR_PREFIX "Blahblah"

void output();
void insert(int, char *);


int main(int argc, char **argv)
{
	int res = 0;
	int cnt = 0;
	item_p item = NULL;
	item_p item_a = NULL;
	item_p item_b = NULL;
	content_t *content = NULL;
	char msg[STR_SIZE]; memset(msg, '\0', STR_SIZE);
	char str_idx[STR_IDX_SIZE]; memset(str_idx, '\0', STR_IDX_SIZE);
	char str_prefix[STR_PREFIX_SIZE]; memset(str_prefix, '\0', STR_PREFIX_SIZE);

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

	fprintf(stdout, "TEST: dequeue__append(content) - NULL != dequeue__first()");
	content = malloc(sizeof(content));
	if (NULL == content) {
		perror("malloc() for content failed");
		exit(EXIT_FAILURE);
	}
	strncpy(content->msg, STR_PREFIX, strlen(STR_PREFIX)+1);
	dequeue__append(content);
	item_a = dequeue__first();
	assert(NULL != item_a);
	fprintf(stdout, " - OK\n");

	fprintf(stdout, "TEST: dequeue__append(content) - NULL != dequeue__last()");
		item_b = dequeue__last();
		assert(NULL != item_b);
		assert(item_a == item_b);
	fprintf(stdout, " - OK\n");

	fprintf(stdout, "TEST: 1 == dequeue__size()");
	res = dequeue__size();
	assert(1 == res);
	fprintf(stdout, " - OK\n");

	fprintf(stdout, "TEST: '%s' == first->content->msg", STR_PREFIX);
	item_a = dequeue__first();
	assert(0 == strcmp(STR_PREFIX, item_a->content->msg));
	fprintf(stdout, " - OK\n");

	fprintf(stdout, "TEST: '%s' == last->content->msg", STR_PREFIX);
	item_b = dequeue__last();
	assert(0 == strcmp(STR_PREFIX, item_b->content->msg));
	fprintf(stdout, " - OK\n");

	fprintf(stdout, "TEST: dequeue__remove(content) - NULL == dequeue__first()");
	free(content); // make sure to free content first!
	dequeue__remove(item_a);
	item_a = dequeue__first();
	assert(NULL == item_a);
	fprintf(stdout, " - OK\n");

	fprintf(stdout, "TEST: dequeue__remove(content) - NULL == dequeue__last()");
	item_b = dequeue__last();
	assert(NULL == item_b);
	assert(item_a == item_b);
	fprintf(stdout, " - OK\n");

	fprintf(stdout, "TEST: 0 == dequeue__size()");
	res = dequeue__size();
	assert(0 == res);
	fprintf(stdout, " - OK\n");

	// TEST: bulk

	// preparation bulk
	fprintf(stdout, "bulk tests preparation\n");
	for (cnt = 0; cnt < NUMBER_OF_ELEMENTS; ++cnt) {
		content = NULL;
		content = malloc(sizeof(*content));
		if (NULL == content) {
			perror("malloc() failed");
			exit(EXIT_FAILURE); // cleaning up already performed allocations
		}
		strcpy(str_prefix, STR_PREFIX);
		strcpy(str_idx, "00");
		sprintf(str_idx, "%d", cnt);
		strcpy(content->msg, strcat(str_prefix, str_idx));
		fprintf(stdout, "\telement #%i added: %s\n", cnt, content->msg);
		dequeue__append(content);
	}


	fprintf(stdout, "TEST dequeue\n\n");

	fprintf(stdout, "TEST: %d == dequeue__size()", NUMBER_OF_ELEMENTS);
	res = dequeue__size();
	assert(NUMBER_OF_ELEMENTS == res);
	fprintf(stdout, " - OK\n");

	fprintf(stdout, "TEST: %d == <iterated through all elements>", NUMBER_OF_ELEMENTS);
	if (NULL != dequeue__first()) {
		cnt = 1;
		for (item = dequeue__first()
			     ; item != dequeue__last()
			     ; item = dequeue__next(item), cnt++)
			;
	}
	assert(res == cnt);
	fprintf(stdout, " - OK\n");

	fprintf(stdout, "TEST: NULL == dequeue__prev(dequeue__first())");
	item = dequeue__prev(dequeue__first());
	assert(NULL == item);
	fprintf(stdout, " - OK\n");

	fprintf(stdout, "TEST: NULL == dequeue__next(dequeue__last())");
	item = dequeue__next(dequeue__last());
	assert(NULL == item);
	fprintf(stdout, " - OK\n");

	item = dequeue__first();
	for (cnt = 0; cnt < dequeue__size(); cnt++) {
		fprintf(stdout, "TEST: %s%d == elem", STR_PREFIX, cnt);
		memset(msg, '\0', STR_SIZE);
		strcpy(str_prefix, STR_PREFIX);
		strcpy(str_idx, "00");
		sprintf(str_idx, "%d", cnt);
		strcpy(msg, strcat(str_prefix, str_idx));
		assert(0 == strcmp(msg, item->content->msg));
		item = dequeue__next(item);
		fprintf(stdout, " - OK\n");
	}


/*
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
