/*
  cunit - tests
 */

#include "linkedlist.h"

#include "test.h"
#include "stdlib.h"
#include "time.h"


void test_list_print(void)
{
	CU_ASSERT(0 == list_size());

	list_insert(97);
	list_insert(48);
	list_insert(21);
	list_insert(66);
	list_insert(81);
	list_insert(57);
	list_insert(33);
	list_insert(45);
	list_insert(19);
	list_insert(31);

	CU_ASSERT(10 == list_size());
	print_dot("list.dot");

	int size = list_size();
	for (int idx = 0; idx < size; idx++) {
		list_delete(list_head());
	}

	CU_ASSERT(0 == list_size());
}

void test_list_empty(void)
{
	int res = FALSE;

	CU_ASSERT(FALSE == res);
	res = list_empty();
	CU_ASSERT(TRUE == res);
}

void test_list_size(void)
{
	CU_ASSERT(0 == list_size());

	list_insert(0);

	CU_ASSERT(1 == list_size());

	list_insert(1);

	CU_ASSERT(2 == list_size());

	list_insert(2);

	CU_ASSERT(3 == list_size());

	list_insert(123);

	CU_ASSERT(4 == list_size());

	list_insert(88);

	CU_ASSERT(5 == list_size());

	list_insert(78);

	CU_ASSERT(6 == list_size());

	list_insert(1);

	CU_ASSERT(7 == list_size());

	list_insert(111);

	CU_ASSERT(8 == list_size());

	list_insert(123);

	CU_ASSERT(9 == list_size());

	list_insert(51);

	CU_ASSERT(10 == list_size());

	int size = list_size();
	for (int idx = 0; idx < size; idx++) {
		list_delete(list_head());
	}
	CU_ASSERT(0 == list_size());
}

void test_list_successor(void)
{
	CU_ASSERT(0 == list_size());

	list_insert(0);
	list_insert(1);
	list_insert(2);

	int size = list_size();
	CU_ASSERT(3 == size);

	if (!list_first()) {
		CU_ASSERT(FALSE);
		return;
	}
	CU_ASSERT(list_first()->next == list_successor(list_first()));
	CU_ASSERT(list_first()->next->next == list_successor(list_first()->next));

	for (int idx = 0; idx < size; idx++) {
		list_delete(list_head());
	}

	CU_ASSERT(0 == list_size());
}

void test_list_predecessor(void)
{
	CU_ASSERT(0 == list_size());

	list_insert(0);
	list_insert(1);
	list_insert(2);

	int size = list_size();
	CU_ASSERT(3 == size);

	if (!list_first()) {
		CU_ASSERT(FALSE);
		return;
	}
	node_p ptr = list_first()->next;
	CU_ASSERT(list_first() == list_predecessor(ptr));
	CU_ASSERT(ptr == list_predecessor(ptr->next));

	for (int idx = 0; idx < size; idx++) {
		list_delete(list_head());
	}

	CU_ASSERT(0 == list_size());
}

void test_list_insert(void)
{
	CU_ASSERT(0 == list_size());

	list_insert(0);
	list_insert(1);
	list_insert(2);
	list_insert(0);
	list_insert(1);
	list_insert(2);
	list_insert(0);
	list_insert(1);
	list_insert(2);
	list_insert(0);

	CU_ASSERT(LIST_MAX_SIZE == list_size());

	list_insert(1);
	list_insert(2);
	CU_ASSERT(LIST_MAX_SIZE == list_size());

	node_p ptr = list_first();
	CU_ASSERT(0 == ptr->data);
	CU_ASSERT(1 == ptr->next->data);
	CU_ASSERT(2 == ptr->next->next->data);

	int size = list_size();
	for (int idx = 0; idx < size; idx++) {
		list_delete(list_head());
	}

	CU_ASSERT(0 == list_size());
}

void test_list_delete(void)
{
	CU_ASSERT(0 == list_size());

	list_insert(0);
	list_insert(1);
	list_insert(2);

	CU_ASSERT(3 == list_size());

	list_delete(list_head());
	list_delete(list_first());
	list_delete(list_head());

	CU_ASSERT(0 == list_size());
}

void test_list_search(void)
{
	CU_ASSERT(0 == list_size());

	list_insert(0);
	list_insert(1);
	list_insert(2);

	int size = list_size();
	CU_ASSERT(3 == size);

	node_p ptr = list_search(2);
	CU_ASSERT(ptr == list_head());

	for (int idx = 0; idx < size; idx++) {
		list_delete(list_head());
	}

	CU_ASSERT(0 == list_size());
}

void test_list_maximum(void)
{
	CU_ASSERT(0 == list_size());

 	list_insert(0);
	list_insert(1);
	list_insert(2);
 	list_insert(3);
	list_insert(5);
	list_insert(8);

	int size = list_size();
	CU_ASSERT(6 == size);

	node_p ptr = list_maximum();
	if (!ptr) {
		CU_ASSERT(FALSE);
	} else {
		CU_ASSERT(8 == ptr->data);
	}

	for (int idx = 0; idx < size; idx++) {
		list_delete(list_head());
	}

	CU_ASSERT(0 == list_size());
}

void test_list_minimum(void)
{
	CU_ASSERT(0 == list_size());

 	list_insert(0);
	list_insert(1);
	list_insert(2);
 	list_insert(3);
	list_insert(5);
	list_insert(8);

	int size = list_size();
	CU_ASSERT(6 == size);

	node_p ptr = list_minimum();
	if (!ptr) {
		CU_ASSERT(FALSE);
	} else {
		CU_ASSERT(0 == ptr->data);
	}

	for (int idx = 0; idx < size; idx++) {
		list_delete(list_head());
	}

	CU_ASSERT(0 == list_size());
}

int main(void)
{
	CU_pSuite pSuite = NULL;

	/* initialize the CUnit test registry */
	if (CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();

	/* add a suite to the registry */
	pSuite = CU_add_suite("linkedlist", init_suite_linkedlist,
			      clean_suite_linkedlist);
	if (NULL == pSuite) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	/* utilities */
	TEST_start(pSuite, "print", test_list_print)
 		TEST_append(pSuite, "empty", test_list_empty)
 		TEST_append(pSuite, "size", test_list_size)
 		TEST_append(pSuite, "successor", test_list_successor)
 		TEST_append(pSuite, "predecessor", test_list_predecessor)
 		TEST_append(pSuite, "insert", test_list_insert)
 		TEST_append(pSuite, "delete", test_list_delete)
 		TEST_append(pSuite, "search", test_list_search)
 		TEST_append(pSuite, "maximum", test_list_maximum)
 		TEST_append(pSuite, "minimum", test_list_minimum)
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
