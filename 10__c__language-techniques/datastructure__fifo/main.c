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
#include "fifo.h"

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
	char msg[STR_SIZE];
	memset(msg, '\0', STR_SIZE);
	char str_idx[STR_IDX_SIZE];
	memset(str_idx, '\0', STR_IDX_SIZE);
	const char str_prefix[] = STR_PREFIX;

	fprintf(stdout, "\nSingle Linked Fifo\n\n");

	mtrace(); /* memory trace */

	for (round = 0; round < nrounds; round++) {
		res = 0;
		cnt = 0;
		item = NULL;
		item_a = NULL;
		item_b = NULL;
		content = NULL;
		memset(msg, '\0', STR_SIZE);
		memset(str_idx, '\0', STR_IDX_SIZE);
		//*
		fprintf(stdout, "TEST: 0 == fifo__size()");
		res = fifo__size();
		assert(0 == res);
		fprintf(stdout, " - OK\n");

		fprintf(stdout, "TEST: NULL == fifo__first()");
		item = fifo__first();
		assert(NULL == item);
		fprintf(stdout, " - OK\n");

		fprintf(stdout, "TEST: NULL == fifo__last()");
		item = fifo__last();
		assert(NULL == item);
		fprintf(stdout, " - OK\n");

		fprintf(stdout,
			"TEST: fifo__push(content) - NULL != fifo__first()");
		content = malloc(sizeof(*content));
		if (NULL == content) {
			perror("malloc() for content failed");
			exit(EXIT_FAILURE);
		}
		content->len = strlen(str_prefix) + 1;
		strncpy(content->msg, str_prefix, content->len);
		fifo__push(content);
		item_a = fifo__first();
		assert(NULL != item_a);
		fprintf(stdout, " - OK\n");

		fprintf(stdout,
			"TEST: fifo__push(content) - NULL != fifo__last()");
		item_b = fifo__last();
		assert(NULL != item_b);
		assert(item_a == item_b);
		fprintf(stdout, " - OK\n");

		fprintf(stdout, "TEST: 1 == fifo__size()");
		res = fifo__size();
		assert(1 == res);
		fprintf(stdout, " - OK\n");

		fprintf(stdout, "TEST: '%s' == first->content->msg",
			str_prefix);
		item_a = fifo__first();
		assert(0 == strcmp(STR_PREFIX, item_a->content->msg));
		fprintf(stdout, " - OK\n");

		fprintf(stdout, "TEST: '%s' == last->content->msg", str_prefix);
		item_b = fifo__last();
		assert(0 == strcmp(STR_PREFIX, item_b->content->msg));
		fprintf(stdout, " - OK\n");

		fprintf(stdout, "TEST: fifo__pop() - NULL == fifo__first()");
		content = fifo__pop();
		assert(0 == strncmp(str_prefix, content->msg, content->len));
		fprintf(stdout, " - OK\n");

		fprintf(stdout, "TEST: fifo__pop() - NULL == fifo__last()");
		item_b = fifo__last();
		assert(NULL == item_b);
		fprintf(stdout, " - OK\n");

		fprintf(stdout, "TEST: 0 == fifo__size()");
		res = fifo__size();
		assert(0 == res);
		fprintf(stdout, " - OK\n");
		// */
		//*
		fprintf(stdout, "\nbulk tests preparation\n\n");
		for (cnt = 0; cnt < NUMBER_OF_ELEMENTS; ++cnt) {
			content = NULL;
			content = malloc(sizeof(*content));
			if (NULL == content) {
				perror("malloc() failed");
				exit(EXIT_FAILURE); // cleaning up already performed allocations
			}
			strcpy(str_idx, "00");
			sprintf(str_idx, "%d", cnt);
			memset(content->msg, '\0', MESSAGE_LENGTH);
			strcat(content->msg, str_prefix);
			strcat(content->msg, str_idx);
			content->len = strlen(content->msg) + 1;
			fprintf(stdout, "\telement #%i added: %s\n", cnt,
				content->msg);
			fifo__push(content);
		}

		fprintf(stdout, "TEST: %d == fifo__size()", NUMBER_OF_ELEMENTS);
		res = fifo__size();
		assert(NUMBER_OF_ELEMENTS == res);
		fprintf(stdout, " - OK\n");

		fprintf(stdout, "TEST: %d == <iterated through all elements>",
			NUMBER_OF_ELEMENTS);
		if (NULL != fifo__first()) {
			cnt = 1;
			for (item = fifo__first(); item != fifo__last();
			     item = fifo__next(item), cnt++)
				;
		}
		assert(res == cnt);
		fprintf(stdout, " - OK\n");

		fprintf(stdout, "TEST: NULL == fifo__prev(fifo__first())");
		item = fifo__prev(fifo__first());
		assert(NULL == item);
		fprintf(stdout, " - OK\n");

		fprintf(stdout, "TEST: NULL == fifo__next(fifo__last())");
		item = fifo__next(fifo__last());
		assert(NULL == item);
		fprintf(stdout, " - OK\n");

		item = fifo__first();
		for (cnt = 0; cnt < fifo__size(); cnt++) {
			fprintf(stdout, "TEST: %s%d == elem", str_prefix, cnt);
			memset(msg, '\0', STR_SIZE);
			strcpy(str_idx, "00");
			sprintf(str_idx, "%d", cnt);
			memset(msg, '\0', MESSAGE_LENGTH);
			strcat(msg, str_prefix);
			strcat(msg, str_idx);
			assert(0 == strcmp(msg, item->content->msg));
			item = fifo__next(item);
			fprintf(stdout, " - OK\n");
		}

		while (fifo__size()) {
			fprintf(stdout, "TEST: fifo__pop() - '%s'",
				fifo__first()->content->msg);
			content = fifo__pop();
			free(content);
			fprintf(stdout, " - OK\n");
		}

		fprintf(stdout, "TEST: 0 == fifo__size()");
		res = fifo__size();
		assert(0 == res);
		fprintf(stdout, " - OK\n");

		fprintf(stdout, "TEST: NULL == fifo__first()");
		item = fifo__first();
		assert(NULL == item);
		fprintf(stdout, " - OK\n");

		fprintf(stdout, "TEST: NULL == fifo__last()");
		item = fifo__last();
		assert(NULL == item);
		fprintf(stdout, " - OK\n");
		// */
	}

	muntrace(); /* memory trace */

	printf("READY.\n");
	exit(EXIT_SUCCESS);
};
