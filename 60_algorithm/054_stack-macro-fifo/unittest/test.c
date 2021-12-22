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
	res = lifo__empty();
	CU_ASSERT(TRUE == res);
}

void test_stack_size(void)
{
	CU_ASSERT(0 == lifo__size());

	lifo__push(0);

	CU_ASSERT(1 == lifo__size());

	lifo__push(1);

	CU_ASSERT(2 == lifo__size());

	lifo__push(2);

	CU_ASSERT(3 == lifo__size());

	lifo__push(123);

	CU_ASSERT(4 == lifo__size());

	lifo__push(88);

	CU_ASSERT(5 == lifo__size());

	lifo__push(78);

	CU_ASSERT(6 == lifo__size());

	lifo__push(1);

	CU_ASSERT(7 == lifo__size());

	lifo__push(111);

	CU_ASSERT(8 == lifo__size());

	lifo__push(123);

	CU_ASSERT(9 == lifo__size());

	lifo__push(51);

	CU_ASSERT(10 == lifo__size());

	int size = lifo__size();
	for (int idx = 0; idx < size; idx++) {
		lifo__pop(NULL);
	}
	CU_ASSERT(0 == lifo__size());
}

void test_stack_successor(void)
{
	CU_ASSERT(0 == lifo__size());

	lifo__push(0);
	lifo__push(1);
	lifo__push(2);

	int size = lifo__size();
	CU_ASSERT(3 == size);

	if (!lifo__first()) {
		CU_ASSERT(FALSE);
		return;
	}
	CU_ASSERT(lifo__first()->next == lifo__successor(lifo__first()));
	CU_ASSERT(lifo__first()->next->next == lifo__successor(lifo__first()->next));

	for (int idx = 0; idx < size; idx++) {
		lifo__pop(NULL);
	}

	CU_ASSERT(0 == lifo__size());
}

void test_stack_predecessor(void)
{
	CU_ASSERT(0 == lifo__size());

	lifo__push(0);
	lifo__push(1);
	lifo__push(2);

	int size = lifo__size();
	CU_ASSERT(3 == size);

	if (!lifo__first()) {
		CU_ASSERT(FALSE);
		return;
	}
	lifo__node_p ptr = lifo__first()->next;
	CU_ASSERT(lifo__first() == lifo__predecessor(ptr));
	CU_ASSERT(ptr == lifo__predecessor(ptr->next));

	for (int idx = 0; idx < size; idx++) {
		lifo__pop(NULL);
	}

	CU_ASSERT(0 == lifo__size());
}

void test_stack_push(void)
{
	CU_ASSERT(0 == lifo__size());

	lifo__push(0);
	lifo__push(1);
	lifo__push(2);
	lifo__push(0);
	lifo__push(1);
	lifo__push(2);
	lifo__push(0);
	lifo__push(1);
	lifo__push(2);
	lifo__push(0);

	CU_ASSERT(lifo__MAX_SIZE == lifo__size());

	lifo__push(1);
	lifo__push(2);
	CU_ASSERT(lifo__MAX_SIZE == lifo__size());

	lifo__node_p ptr = lifo__first();
	CU_ASSERT(0 == ptr->data);
	CU_ASSERT(1 == ptr->next->data);
	CU_ASSERT(2 == ptr->next->next->data);

	int size = lifo__size();
	for (int idx = 0; idx < size; idx++) {
		lifo__pop(NULL);
	}

	CU_ASSERT(0 == lifo__size());
}

void test_stack_pop(void)
{
	int data = -1;
	int arr[] = { 55, 41, 67, 29, 43, 81, 37, 14, 9, 25 };
	int size = sizeof(arr) / sizeof(*arr);

	CU_ASSERT(0 == lifo__size());

	for (int idx = 0; idx < size; idx++) {
		lifo__push(arr[idx]);
	}

	CU_ASSERT(size == lifo__size());

	lifo__push(123);
	CU_ASSERT(lifo__MAX_SIZE == lifo__size());

	for (int idx = size-1; idx >= 0; idx--) {
		lifo__pop(&data);
		CU_ASSERT(arr[idx] == data);
	}

	CU_ASSERT(0 == lifo__size());
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
