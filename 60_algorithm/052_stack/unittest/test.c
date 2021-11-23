/*
  cunit - tests
 */

#include "stack.h"

#include "test.h"
#include "stdlib.h"
#include "time.h"


void test_stack_empty(void)
{
	int res = FALSE;

	CU_ASSERT(FALSE == res);
	res = stack_empty();
	CU_ASSERT(TRUE == res);
}

void test_stack_size(void)
{
	CU_ASSERT(0 == stack_size());

	stack_push(0);

	CU_ASSERT(1 == stack_size());

	stack_push(1);

	CU_ASSERT(2 == stack_size());

	stack_push(2);

	CU_ASSERT(3 == stack_size());

	stack_push(123);

	CU_ASSERT(4 == stack_size());

	stack_push(88);

	CU_ASSERT(5 == stack_size());

	stack_push(78);

	CU_ASSERT(6 == stack_size());

	stack_push(1);

	CU_ASSERT(7 == stack_size());

	stack_push(111);

	CU_ASSERT(8 == stack_size());

	stack_push(123);

	CU_ASSERT(9 == stack_size());

	stack_push(51);

	CU_ASSERT(10 == stack_size());

	int size = stack_size();
	for (int idx = 0; idx < size; idx++) {
		stack_pop();
	}
	CU_ASSERT(0 == stack_size());
}

void test_stack_successor(void)
{
	CU_ASSERT(0 == stack_size());

	stack_push(0);
	stack_push(1);
	stack_push(2);

	int size = stack_size();
	CU_ASSERT(3 == size);

	if (!first) {
		CU_ASSERT(FALSE);
		return;
	}
	CU_ASSERT(first->next == stack_successor(first));
	CU_ASSERT(first->next->next == stack_successor(first->next));

	for (int idx = 0; idx < size; idx++) {
		stack_pop();
	}

	CU_ASSERT(0 == stack_size());
}

void test_stack_predecessor(void)
{
	CU_ASSERT(0 == stack_size());

	stack_push(0);
	stack_push(1);
	stack_push(2);

	int size = stack_size();
	CU_ASSERT(3 == size);

	if (!first) {
		CU_ASSERT(FALSE);
		return;
	}
	node_p ptr = first->next;
	CU_ASSERT(first == stack_predecessor(ptr));
	CU_ASSERT(ptr == stack_predecessor(ptr->next));

	for (int idx = 0; idx < size; idx++) {
		stack_pop();
	}

	CU_ASSERT(0 == stack_size());
}

void test_stack_push(void)
{
	CU_ASSERT(0 == stack_size());

	stack_push(0);
	stack_push(1);
	stack_push(2);
	stack_push(0);
	stack_push(1);
	stack_push(2);
	stack_push(0);
	stack_push(1);
	stack_push(2);
	stack_push(0);

	CU_ASSERT(LIST_MAX_SIZE == stack_size());

	stack_push(1);
	stack_push(2);
	CU_ASSERT(LIST_MAX_SIZE == stack_size());

	node_p ptr = first;
	CU_ASSERT(0 == ptr->data);
	CU_ASSERT(1 == ptr->next->data);
	CU_ASSERT(2 == ptr->next->next->data);

	int size = stack_size();
	for (int idx = 0; idx < size; idx++) {
		stack_pop();
	}

	CU_ASSERT(0 == stack_size());
}

void test_stack_pop(void)
{
	int data = -1;
	int arr[] = { 55, 41, 67, 29, 43, 81, 37, 14, 9, 25 };
	int size = sizeof(arr) / sizeof(*arr);

	CU_ASSERT(0 == stack_size());

	for (int idx = 0; idx < size; idx++) {
		stack_push(arr[idx]);
	}

	CU_ASSERT(size == stack_size());

	stack_push(123);
	CU_ASSERT(LIST_MAX_SIZE == stack_size());

	for (int idx = size-1; idx >= 0; idx--) {
		data = stack_pop();
		CU_ASSERT(arr[idx] == data);
	}

	CU_ASSERT(0 == stack_size());
}

void test_stack_search(void)
{
	CU_ASSERT(0 == stack_size());

	stack_push(0);
	stack_push(1);
	stack_push(2);

	int size = stack_size();
	CU_ASSERT(3 == size);

	node_p ptr = stack_search(2);
	CU_ASSERT(ptr == head);

	for (int idx = 0; idx < size; idx++) {
		stack_pop();
	}

	CU_ASSERT(0 == stack_size());
}

void test_stack_maximum(void)
{
	CU_ASSERT(0 == stack_size());

 	stack_push(0);
	stack_push(1);
	stack_push(2);
 	stack_push(3);
	stack_push(5);
	stack_push(8);

	int size = stack_size();
	CU_ASSERT(6 == size);

	node_p ptr = stack_maximum();
	if (!ptr) {
		CU_ASSERT(FALSE);
	} else {
		CU_ASSERT(8 == ptr->data);
	}

	for (int idx = 0; idx < size; idx++) {
		stack_pop();
	}

	CU_ASSERT(0 == stack_size());
}

void test_stack_minimum(void)
{
	CU_ASSERT(0 == stack_size());

 	stack_push(0);
	stack_push(1);
	stack_push(2);
 	stack_push(3);
	stack_push(5);
	stack_push(8);

	int size = stack_size();
	CU_ASSERT(6 == size);

	node_p ptr = stack_minimum();
	if (!ptr) {
		CU_ASSERT(FALSE);
	} else {
		CU_ASSERT(0 == ptr->data);
	}

	for (int idx = 0; idx < size; idx++) {
		stack_pop();
	}

	CU_ASSERT(0 == stack_size());
}

int main(void)
{
	CU_pSuite pSuite = NULL;

	/* initialize the CUnit test registry */
	if (CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();

	/* add a suite to the registry */
	pSuite = CU_add_suite("stack", init_suite_stack,
			      clean_suite_stack);
	if (NULL == pSuite) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	/* utilities */
	TEST_start(pSuite, "empty", test_stack_empty)
 		TEST_append(pSuite, "size", test_stack_size)
 		TEST_append(pSuite, "successor", test_stack_successor)
 		TEST_append(pSuite, "predecessor", test_stack_predecessor)
 		TEST_append(pSuite, "push", test_stack_push)
 		TEST_append(pSuite, "pop", test_stack_pop)
 		TEST_append(pSuite, "search", test_stack_search)
 		TEST_append(pSuite, "maximum", test_stack_maximum)
 		TEST_append(pSuite, "minimum", test_stack_minimum)
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
