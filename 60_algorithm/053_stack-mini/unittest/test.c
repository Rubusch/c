/*
  cunit - tests
 */

#include "stack.h"

#include "test.h"
#include "stdlib.h"
#include "time.h"


void test_stack_print(void)
{
	CU_ASSERT(0 == stack_size());

	stack_push(97);
	stack_push(48);
	stack_push(21);
	stack_push(66);
	stack_push(81);
	stack_push(57);
	stack_push(33);
	stack_push(45);
	stack_push(19);
	stack_push(31);

	CU_ASSERT(10 == stack_size());
	print_dot("stack.dot");

	int size = stack_size();
	for (int idx = 0; idx < size; idx++) {
		stack_pop();
	}

	CU_ASSERT(0 == stack_size());
}

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

	if (!stack_first()) {
		CU_ASSERT(FALSE);
		return;
	}
	CU_ASSERT(stack_first()->next == stack_successor(stack_first()));
	CU_ASSERT(stack_first()->next->next == stack_successor(stack_first()->next));

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

	if (!stack_first()) {
		CU_ASSERT(FALSE);
		return;
	}
	node_p ptr = stack_first()->next;
	CU_ASSERT(stack_first() == stack_predecessor(ptr));
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

	CU_ASSERT(STACK_MAX_SIZE == stack_size());

	stack_push(1);
	stack_push(2);
	CU_ASSERT(STACK_MAX_SIZE == stack_size());

	node_p ptr = stack_first();
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
	CU_ASSERT(STACK_MAX_SIZE == stack_size());

	for (int idx = size-1; idx >= 0; idx--) {
		data = stack_pop();
		CU_ASSERT(arr[idx] == data);
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
	TEST_start(pSuite, "print", test_stack_print)
 		TEST_append(pSuite, "empty", test_stack_empty)
 		TEST_append(pSuite, "size", test_stack_size)
 		TEST_append(pSuite, "successor", test_stack_successor)
 		TEST_append(pSuite, "predecessor", test_stack_predecessor)
 		TEST_append(pSuite, "push", test_stack_push)
 		TEST_append(pSuite, "pop", test_stack_pop)
	TEST_end();

	CU_basic_set_mode(CU_BRM_VERBOSE);
#if defined BASICTEST
	CU_basic_run_tests();
#else
	CU_curses_run_tests();
#endif
	fprintf(stderr, "\n");
	CU_basic_show_failures(CU_get_failure_list());
	fprintf(stderr, "\n\n");

	CU_cleanup_registry();
	return CU_get_error();
}
