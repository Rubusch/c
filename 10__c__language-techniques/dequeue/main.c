// main.c
/*
 * @author: Lothar Rubusch
 * @email: L.Rubusch@gmx.ch
 * @license: GPLv3
 * @date: 2013-april-28
 */
/*
  debugging malloc allocations

  ressource: http://en.wikipedia.org/wiki/Mtrace

  1. compile with
  $ gcc -g ...

  2. set path to log output
  $ export MALLOC_TRACE=./out.log

  3. start program
  $ ./program.exe

  4. then call
  $ mtrace ./program.exe ./out.log

  mtrace will show where memory gets allocated that is NOT freed properly
*/

#define _XOPEN_SOURCE 600
#define _XOPEN_SOURCE_EXTENDED


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <assert.h>
#include <mcheck.h>

#include "content.h"
#include "dequeue_item.h"

#define NUMBER_OF_ELEMENTS 10
#define STR_SIZE 32
#define STR_IDX_SIZE 8
#define STR_PREFIX_SIZE 10
#define STR_PREFIX "Blahblah"

void output();
void insert(int, char *);


int main(int argc, char **argv)
{
	int round = 0;
	int nrounds = 2;
	int res = 0;
	int cnt = 0;
	item_p item = NULL;
	item_p item_a = NULL;
	item_p item_b = NULL;
	content_p content = NULL;
	char msg[STR_SIZE]; memset(msg, '\0', STR_SIZE);
	char str_idx[STR_IDX_SIZE]; memset(str_idx, '\0', STR_IDX_SIZE);
//	char str_prefix[STR_PREFIX_SIZE]; memset(str_prefix, '\0', STR_PREFIX_SIZE);
	const char str_prefix[] = STR_PREFIX;

	fprintf(stdout, "\nSingle Linked Dequeue\n\n");

	mtrace(); /* memory trace */

	for (round=0; round < nrounds; round++) {
		res = 0;
		cnt = 0;
		item = NULL;
		item_a = NULL;
		item_b = NULL;
		content = NULL;
		memset(msg, '\0', STR_SIZE);
		memset(str_idx, '\0', STR_IDX_SIZE);
//		memset(str_prefix, '\0', STR_PREFIX_SIZE);
//		strcpy(str_prefix, STR_PREFIX); // TODO rm
//*
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
//		content = malloc(sizeof(*content));
		content = malloc(sizeof(struct content_s));
		if (NULL == content) {
			perror("malloc() for content failed");
			exit(EXIT_FAILURE);
		}
//		content->len = strlen(STR_PREFIX)+1;
		content->len = strlen(str_prefix)+1;
//		strncpy(content->msg, STR_PREFIX, content->len);  
		strncpy(content->msg, str_prefix, content->len);
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

//		fprintf(stdout, "TEST: '%s' == first->content->msg", STR_PREFIX); // TODO rm  
		fprintf(stdout, "TEST: '%s' == first->content->msg", str_prefix);
		item_a = dequeue__first();
		assert(0 == strcmp(STR_PREFIX, item_a->content->msg));
		fprintf(stdout, " - OK\n");

//		fprintf(stdout, "TEST: '%s' == last->content->msg", STR_PREFIX); // TODO rm  
		fprintf(stdout, "TEST: '%s' == last->content->msg", str_prefix);
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
// */
//*
		fprintf(stdout, "\nbulk tests preparation\n\n");
		for (cnt = 0; cnt < NUMBER_OF_ELEMENTS; ++cnt) {
			content = NULL;
//			content = malloc(sizeof(*content));
			content = malloc(sizeof(struct content_s));
			if (NULL == content) {
				perror("malloc() failed");
				exit(EXIT_FAILURE); // cleaning up already performed allocations
			}
//			strcpy(str_prefix, STR_PREFIX); // TODO rm
			strcpy(str_idx, "00");
			sprintf(str_idx, "%d", cnt);
//			strcpy(content->msg, strcat(str_prefix, str_idx)); // TODO rm
			memset(content->msg, '\0', MESSAGE_LENGTH);
			strcat(content->msg, str_prefix);
			strcat(content->msg, str_idx);
			content->len = strlen(content->msg)+1;
			fprintf(stdout, "\telement #%i added: %s\n", cnt, content->msg);
			dequeue__append(content);
		}

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
//			fprintf(stdout, "TEST: %s%d == elem", STR_PREFIX, cnt); // TODO rm  
			fprintf(stdout, "TEST: %s%d == elem", str_prefix, cnt);
			memset(msg, '\0', STR_SIZE);
//			strcpy(str_prefix, STR_PREFIX); // TODO rm  
			strcpy(str_idx, "00");
			sprintf(str_idx, "%d", cnt);
//			strcpy(msg, strcat(str_prefix, str_idx)); // TODO rm  
			memset(msg, '\0', MESSAGE_LENGTH);
			strcat(msg, str_prefix);
			strcat(msg, str_idx);
			assert(0 == strcmp(msg, item->content->msg));
			item = dequeue__next(item);
			fprintf(stdout, " - OK\n");
		}

		while (dequeue__size()) {
			fprintf(stdout, "TEST: dequeue__remove(first) - '%s'", dequeue__first()->content->msg);
			item = dequeue__first();
			free(item->content); // make sure to free content first!
			item->content = NULL;
			dequeue__remove(item);
			item = dequeue__first();
			if (dequeue__first() != dequeue__last())
				assert(NULL != item);
			fprintf(stdout, " - OK\n");
		}

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
// */
//*
		fprintf(stdout, "\ndequeue insertion\n\n");
		for (cnt = 0; cnt < NUMBER_OF_ELEMENTS; ++cnt) {
			content = NULL;
//			content = malloc(sizeof(*content));
			content = malloc(sizeof(struct content_s));
			if (NULL == content) {
				perror("malloc() failed");
				exit(EXIT_FAILURE); // cleaning up already performed allocations
			}
//			strcpy(str_prefix, STR_PREFIX); // TODO rm
			strcpy(str_idx, "00");
			sprintf(str_idx, "%d", cnt);
			//strcpy(content->msg, strcat(str_prefix, str_idx)); // TODO rm
			memset(content->msg, '\0', MESSAGE_LENGTH);
			strcat(content->msg, str_prefix);
			strcat(content->msg, str_idx);
			content->len = strlen(content->msg)+1;
			fprintf(stdout, "\telement #%i added: %s\n", cnt, content->msg);
			dequeue__append(content);
		}

		fprintf(stdout, "TEST: NULL == dequeue__find(%s)", "AAA");
		content = NULL;
//		content = malloc(sizeof(*content));
		content = malloc(sizeof(struct content_s));
		if (NULL == content) {
			perror("malloc() failed");
			exit(EXIT_FAILURE); // cleaning up already performed allocations
		}
		strcpy(content->msg, "AAA");
		content->len = strlen(content->msg)+1;
		item = dequeue__find(content);
		free(content);
		assert(NULL == item);
		fprintf(stdout, " - OK\n");
//*
		// append "AAA" after element containing "Blahblah7"
		fprintf(stdout, "TEST: '%d' == dequeue__size()", NUMBER_OF_ELEMENTS+1);
		content = NULL;
//		content = malloc(sizeof(*content));
		content = malloc(sizeof(struct content_s));
		if (NULL == content) {
			perror("malloc() failed");
			exit(EXIT_FAILURE); // cleaning up already performed allocations
		}
		cnt = 7;
		memset(msg, '\0', STR_SIZE);
//		strcpy(str_prefix, STR_PREFIX); // TODO rm
		strcpy(str_idx, "00");
		sprintf(str_idx, "%d", cnt);
//		strcpy(content->msg, strcat(str_prefix, str_idx)); // TODO rm
		memset(content->msg, '\0', MESSAGE_LENGTH);
		strcat(content->msg, str_prefix);
		strcat(content->msg, str_idx);
		content->len = strlen(content->msg)+1;
		item = dequeue__find(content);
		free(content);
		content = NULL;
//		content = malloc(sizeof(*content));
		content = malloc(sizeof(struct content_s));
		if (NULL == content) {
			perror("malloc() failed");
			exit(EXIT_FAILURE); // cleaning up already performed allocations
		}
		strcpy(content->msg, "AAA");
		content->len = strlen(content->msg)+1;
		dequeue__insert_after(item, content);
		assert(NUMBER_OF_ELEMENTS+1 == dequeue__size());
		fprintf(stdout, " - OK\n");

		fprintf(stdout, "TEST: NULL != dequeue__find(%s)", "AAA");
		content = NULL;
//		content = malloc(sizeof(*content));
		content = malloc(sizeof(struct content_s));
		if (NULL == content) {
			perror("malloc() failed");
			exit(EXIT_FAILURE); // cleaning up already performed allocations
		}
		strcpy(content->msg, "AAA");
		content->len = strlen(content->msg)+1;
		item = dequeue__find(content);
		free(content);
		assert(NULL != item);
		fprintf(stdout, " - OK\n");
//*
		fprintf(stdout, "TEST: dequeue__remove(content) - '%s'", "AAA");
		content = NULL;
//		content = malloc(sizeof(*content));
		content = malloc(sizeof(struct content_s));
		if (NULL == content) {
			perror("malloc() failed");
			exit(EXIT_FAILURE); // cleaning up already performed allocations
		}
		strcpy(content->msg, "AAA");
		content->len = strlen(content->msg)+1;
		item = dequeue__find(content);
		free(content);
		content = NULL;
		free(item->content); // make sure to free content first!
		item->content=NULL;
		dequeue__remove(item);
		item = dequeue__first();
		if (dequeue__first() != dequeue__last())
			assert(NULL != item);
		fprintf(stdout, " - OK\n");

		fprintf(stdout, "TEST: '%d' == dequeue__size()", NUMBER_OF_ELEMENTS);
		assert(NUMBER_OF_ELEMENTS == dequeue__size());
		fprintf(stdout, " - OK\n");
// */
		while (dequeue__size()) {
			fprintf(stdout, "TEST: dequeue__remove(first) - '%s'", dequeue__first()->content->msg);
			item = dequeue__first();
			content = item->content;
			free(content); // make sure to free content first!
			dequeue__remove(item);
			item = dequeue__first();
			if (dequeue__first() != dequeue__last())
				assert(NULL != item);
			fprintf(stdout, " - OK\n");
		}
// */
	}

	muntrace(); /* memory trace */

	printf("READY.\n");
	exit(EXIT_SUCCESS);
};
