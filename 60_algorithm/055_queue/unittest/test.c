/*
  cunit - tests
 */

#include "queue.h"

#include "test.h"
#include "stdlib.h"
#include "time.h"


void test_queue_print(void)
{
	CU_ASSERT(0 == queue_size());

	queue_enqueue(97);
	queue_enqueue(48);
	queue_enqueue(21);
	queue_enqueue(66);
	queue_enqueue(81);
	queue_enqueue(57);
	queue_enqueue(33);
	queue_enqueue(45);
	queue_enqueue(19);
	queue_enqueue(31);

	CU_ASSERT(10 == queue_size());
	print_dot("queue.dot");

	int size = queue_size();
	for (int idx = 0; idx < size; idx++) {
		queue_dequeue();
	}

	CU_ASSERT(0 == queue_size());
}

void test_queue_empty(void)
{
	int res = FALSE;

	CU_ASSERT(FALSE == res);
	res = queue_empty();
	CU_ASSERT(TRUE == res);
}

void test_queue_size(void)
{
	CU_ASSERT(0 == queue_size());

	queue_enqueue(0);

	CU_ASSERT(1 == queue_size());

	queue_enqueue(1);

	CU_ASSERT(2 == queue_size());

	queue_enqueue(2);

	CU_ASSERT(3 == queue_size());

	queue_enqueue(123);

	CU_ASSERT(4 == queue_size());

	queue_enqueue(88);

	CU_ASSERT(5 == queue_size());

	queue_enqueue(78);

	CU_ASSERT(6 == queue_size());

	queue_enqueue(1);

	CU_ASSERT(7 == queue_size());

	queue_enqueue(111);

	CU_ASSERT(8 == queue_size());

	queue_enqueue(123);

	CU_ASSERT(9 == queue_size());

	queue_enqueue(51);

	CU_ASSERT(10 == queue_size());

	int size = queue_size();
	for (int idx = 0; idx < size; idx++) {
		queue_dequeue();
	}
	CU_ASSERT(0 == queue_size());
}

void test_queue_successor(void)
{
	CU_ASSERT(0 == queue_size());

	queue_enqueue(0);
	queue_enqueue(1);
	queue_enqueue(2);

	int size = queue_size();
	CU_ASSERT(3 == size);

	if (!queue_first()) {
		CU_ASSERT(FALSE);
		return;
	}
	CU_ASSERT(queue_first()->next == queue_successor(queue_first()));
	CU_ASSERT(queue_first()->next->next == queue_successor(queue_first()->next));

	for (int idx = 0; idx < size; idx++) {
		queue_dequeue();
	}

	CU_ASSERT(0 == queue_size());
}

void test_queue_predecessor(void)
{
	CU_ASSERT(0 == queue_size());

	queue_enqueue(0);
	queue_enqueue(1);
	queue_enqueue(2);

	int size = queue_size();
	CU_ASSERT(3 == size);

	if (!queue_first()) {
		CU_ASSERT(FALSE);
		return;
	}
	node_p ptr = queue_first()->next;
	CU_ASSERT(queue_first() == queue_predecessor(ptr));
	CU_ASSERT(ptr == queue_predecessor(ptr->next));

	for (int idx = 0; idx < size; idx++) {
		queue_dequeue();
	}

	CU_ASSERT(0 == queue_size());
}

void test_queue_enqueue(void)
{
	CU_ASSERT(0 == queue_size());

	queue_enqueue(0);
	queue_enqueue(1);
	queue_enqueue(2);
	queue_enqueue(0);
	queue_enqueue(1);
	queue_enqueue(2);
	queue_enqueue(0);
	queue_enqueue(1);
	queue_enqueue(2);
	queue_enqueue(0);

	CU_ASSERT(LIST_MAX_SIZE == queue_size());

	queue_enqueue(1);
	queue_enqueue(2);
	CU_ASSERT(LIST_MAX_SIZE == queue_size());

	node_p ptr = queue_first();
	CU_ASSERT(0 == ptr->data);
	CU_ASSERT(1 == ptr->next->data);
	CU_ASSERT(2 == ptr->next->next->data);

	int size = queue_size();
	for (int idx = 0; idx < size; idx++) {
		queue_dequeue();
	}

	CU_ASSERT(0 == queue_size());
}

void test_queue_dequeue(void)
{
	int data = -1;
	int arr[] = { 55, 41, 67, 29, 43, 81, 37, 14, 9, 25 };
	int size = sizeof(arr) / sizeof(*arr);

	CU_ASSERT(0 == queue_size());

	for (int idx = 0; idx < size; idx++) {
		queue_enqueue(arr[idx]);
	}

	CU_ASSERT(size == queue_size());

	queue_enqueue(123);
	CU_ASSERT(LIST_MAX_SIZE == queue_size());

	for (int idx = 0; idx < size; idx++) {
		data = queue_dequeue();
		CU_ASSERT(arr[idx] == data);
	}

	CU_ASSERT(0 == queue_size());
}

void test_queue_search(void)
{
	CU_ASSERT(0 == queue_size());

	queue_enqueue(0);
	queue_enqueue(1);
	queue_enqueue(2);

	int size = queue_size();
	CU_ASSERT(3 == size);

	node_p ptr = queue_search(2);
	CU_ASSERT(ptr == queue_head());

	for (int idx = 0; idx < size; idx++) {
		queue_dequeue();
	}

	CU_ASSERT(0 == queue_size());
}

void test_queue_maximum(void)
{
	CU_ASSERT(0 == queue_size());

 	queue_enqueue(0);
	queue_enqueue(1);
	queue_enqueue(2);
 	queue_enqueue(3);
	queue_enqueue(5);
	queue_enqueue(8);

	int size = queue_size();
	CU_ASSERT(6 == size);

	node_p ptr = queue_maximum();
	if (!ptr) {
		CU_ASSERT(FALSE);
	} else {
		CU_ASSERT(8 == ptr->data);
	}

	for (int idx = 0; idx < size; idx++) {
		queue_dequeue();
	}

	CU_ASSERT(0 == queue_size());
}

void test_queue_minimum(void)
{
	CU_ASSERT(0 == queue_size());

 	queue_enqueue(0);
	queue_enqueue(1);
	queue_enqueue(2);
 	queue_enqueue(3);
	queue_enqueue(5);
	queue_enqueue(8);

	int size = queue_size();
	CU_ASSERT(6 == size);

	node_p ptr = queue_minimum();
	if (!ptr) {
		CU_ASSERT(FALSE);
	} else {
		CU_ASSERT(0 == ptr->data);
	}

	for (int idx = 0; idx < size; idx++) {
		queue_dequeue();
	}

	CU_ASSERT(0 == queue_size());
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
	TEST_start(pSuite, "print", test_queue_print)
 		TEST_append(pSuite, "empty", test_queue_empty)
 		TEST_append(pSuite, "size", test_queue_size)
 		TEST_append(pSuite, "successor", test_queue_successor)
 		TEST_append(pSuite, "predecessor", test_queue_predecessor)
 		TEST_append(pSuite, "enqueue", test_queue_enqueue)
 		TEST_append(pSuite, "dequeue", test_queue_dequeue)
 		TEST_append(pSuite, "search", test_queue_search)
 		TEST_append(pSuite, "maximum", test_queue_maximum)
 		TEST_append(pSuite, "minimum", test_queue_minimum)
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
