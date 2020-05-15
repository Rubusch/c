// main.c
/*
  main file to start the proggy

  the preprocessor concatenation can be used to generate a fixed data structure
  implementation, make it prefix with an identifier and it allows for extending
  individually the list instances

  the drawbacks will be debugging inside the data structure, anyway the data
  strucutre can mature to a decent level as long as it is simple enough, so this
  could be a reasonable solution in some applications

  (the question if putting entire data structures into pre-processor space is
  generally a good design or a weakness can be a major criticism)
//*/

#include <stdio.h>
#include <stdlib.h>

#include "macrolist.h"

/*
  set include guards at definition, e.g. here - better, put
  the CREATE_LIST() directly in the corresponding header of
  THIS file
*/
#ifndef SOME_LIST

#define SOME_LIST
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"

CREATE_LIST(some)

#pragma GCC diagnostic pop
#endif



#ifndef UNIQUE_LIST

#define UNIQUE_LIST
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"


CREATE_LIST(unique)

/* add additional functions to the data structure */
static int unique__remove_duplicates()
{
	unique_t *elem = unique__first;

	if (!unique__first)
		return -1;

	for (elem = unique__first; elem->next != NULL; elem = elem->next) {
		for (unique_t *elem__runner = elem; elem__runner->next != NULL; elem__runner = elem__runner->next) {
			if (0 == strcmp(elem->data, elem__runner->data)) {
				elem = elem->next;
				unique__remove(elem__runner->data);
				if (!elem->next)
					goto done;
				break;
			}
		}
	}
	done:

	return 0;
}

#pragma GCC diagnostic pop
#endif /* UNIQUE_LIST */


int main()
{
	/* first list */
	puts("uninitialized");
	some__print();
	printf("expected: [empty]\n\n");

	puts("init 1");
	some__append("AAA");
	some__print();
	printf("expected: [AAA]\n\n");

	puts("append another element");
	some__append("BBB");
	some__print();
	printf("expected: [AAA, BBB]\n\n");

	puts("append another element");
	some__append("CCC");
	some__print();
	printf("expected: [AAA, BBB, CCC]\n\n");

	puts("remove element");
	some__remove("BBB");
	some__print();
	printf("expected: [AAA, CCC]\n\n");

	puts("remove non-existend element");
	some__remove("BBB");
	some__print();
	printf("expected: [AAA, CCC]\n\n");

	puts("remove all some");
	some__removeall();
	some__print();
	printf("expected: [empty]\n\n");


	/* second list */
	puts("init by 3 elements");
	unique__append("AAA");
	unique__append("BBB");
	unique__append("AAA");
	unique__print();
	printf("expected: [AAA, BBB, AAA]\n\n");

	puts("remove duplicates");
	unique__remove_duplicates();
	unique__print();
	printf("expected: [AAA, BBB]\n\n");


	exit(EXIT_FAILURE);
}
