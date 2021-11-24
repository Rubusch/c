/*
  cunit - tests
 */

#include "queue.h"

#include "test.h"
#include "stdlib.h"
#include "time.h"


void test_queue_empty(void)
{
	int res = FALSE;

	CU_ASSERT(FALSE == res);
	res = fifo__empty();
	CU_ASSERT(TRUE == res);
}

void test_queue_size(void)
{
	CU_ASSERT(0 == fifo__size());

	fifo__enqueue(0);

	CU_ASSERT(1 == fifo__size());

	fifo__enqueue(1);

	CU_ASSERT(2 == fifo__size());

	fifo__enqueue(2);

	CU_ASSERT(3 == fifo__size());

	fifo__enqueue(123);

	CU_ASSERT(4 == fifo__size());

	fifo__enqueue(88);

	CU_ASSERT(5 == fifo__size());

	fifo__enqueue(78);

	CU_ASSERT(6 == fifo__size());

	fifo__enqueue(1);

	CU_ASSERT(7 == fifo__size());

	fifo__enqueue(111);

	CU_ASSERT(8 == fifo__size());

	fifo__enqueue(123);

	CU_ASSERT(9 == fifo__size());

	fifo__enqueue(51);

	CU_ASSERT(10 == fifo__size());

	int size = fifo__size();
	for (int idx = 0; idx < size; idx++) {
		fifo__dequeue(NULL);
	}
	CU_ASSERT(0 == fifo__size());
}

void test_queue_successor(void)
{
	CU_ASSERT(0 == fifo__size());

	fifo__enqueue(0);
	fifo__enqueue(1);
	fifo__enqueue(2);

	int size = fifo__size();
	CU_ASSERT(3 == size);

	if (!fifo__first()) {
		CU_ASSERT(FALSE);
		return;
	}
	CU_ASSERT(fifo__first()->next == fifo__successor(fifo__first()));
	CU_ASSERT(fifo__first()->next->next == fifo__successor(fifo__first()->next));

	for (int idx = 0; idx < size; idx++) {
		fifo__dequeue(NULL);
	}

	CU_ASSERT(0 == fifo__size());
}

void test_queue_predecessor(void)
{
	CU_ASSERT(0 == fifo__size());

	fifo__enqueue(0);
	fifo__enqueue(1);
	fifo__enqueue(2);

	int size = fifo__size();
	CU_ASSERT(3 == size);

	if (!fifo__first()) {
		CU_ASSERT(FALSE);
		return;
	}
	fifo__node_p ptr = fifo__first()->next;
	CU_ASSERT(fifo__first() == fifo__predecessor(ptr));
	CU_ASSERT(ptr == fifo__predecessor(ptr->next));

	for (int idx = 0; idx < size; idx++) {
		fifo__dequeue(NULL);
	}

	CU_ASSERT(0 == fifo__size());
}

void test_queue_enqueue(void)
{
	CU_ASSERT(0 == fifo__size());

	for (int idx = 0; idx < fifo__MAX_SIZE; idx++) {
		fifo__enqueue(idx);
	}
	CU_ASSERT(fifo__MAX_SIZE == fifo__size());

	fifo__enqueue(1);
	fifo__enqueue(2);
	CU_ASSERT(fifo__MAX_SIZE == fifo__size());

	fifo__node_p ptr = fifo__first();
	CU_ASSERT(0 == ptr->data);
	CU_ASSERT(1 == ptr->next->data);
	CU_ASSERT(2 == ptr->next->next->data);

	int size = fifo__size();
	for (int idx = 0; idx < size; idx++) {
		fifo__dequeue(NULL);
	}

	CU_ASSERT(0 == fifo__size());
}

void test_queue_dequeue(void)
{
	int data = -1;
	int arr[] = { 55, 41, 67, 29, 43, 81, 37, 14, 9, 25 };
	int size = sizeof(arr) / sizeof(*arr);

	CU_ASSERT(0 == fifo__size());

	for (int idx = 0; idx < size; idx++) {
		fifo__enqueue(arr[idx]);
	}

	CU_ASSERT(size == fifo__size());

	for (int idx = 0; idx < size; idx++) {
		fifo__dequeue(&data);
		CU_ASSERT(arr[idx] == data);
	}

	CU_ASSERT(0 == fifo__size());
}


int main(void)
{
	CU_pSuite pSuite = NULL;

	/* initialize the CUnit test registry */
	if (CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();

	/* add a suite to the registry */
	pSuite = CU_add_suite("queue", init_suite_queue,
			      clean_suite_queue);
	if (NULL == pSuite) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	/* utilities */
	TEST_start(pSuite, "empty", test_queue_empty)
 		TEST_append(pSuite, "size", test_queue_size)
 		TEST_append(pSuite, "successor", test_queue_successor)
 		TEST_append(pSuite, "predecessor", test_queue_predecessor)
 		TEST_append(pSuite, "enqueue", test_queue_enqueue)
 		TEST_append(pSuite, "dequeue", test_queue_dequeue)
	TEST_end();

	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	fprintf(stderr, "\n");
	CU_basic_show_failures(CU_get_failure_list());
	fprintf(stderr, "\n\n");

#if defined BASICTEST
	CU_automated_run_tests();
#else
	CU_curses_run_tests();
#endif

	/* clean up registry and return */
	CU_cleanup_registry();
	return CU_get_error();
}
