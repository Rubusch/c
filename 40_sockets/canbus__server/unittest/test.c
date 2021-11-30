/*
  cunit - tests
 */

#include "can_interface.h"

#include "test.h"
#include "stdlib.h"
#include "time.h"


void test_queue_empty(void)
{
	int res = FALSE;
	CU_ASSERT(FALSE == res);
	res = tx__empty();
	CU_ASSERT(TRUE == res);
	res = rx__empty();
	CU_ASSERT(TRUE == res);
}

void test_tx(void)
{
	canif__startup("MickeyMouse", strlen("MickeyMouse") + 1);

	// TODO
}

void test_rx(void)
{
	// TODO
}

// */

int main(void)
{
	CU_pSuite pSuite = NULL;

	/* initialize the CUnit test registry */
	if (CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();

	/* add a suite to the registry */
	pSuite = CU_add_suite("queue", init_suite_canif, clean_suite_canif);
	if (NULL == pSuite) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	/* utilities */
	TEST_start(pSuite, "empty queues", test_queue_empty)
/* 		TEST_append(pSuite, "tx", test_tx) // */
/* 		TEST_append(pSuite, "rx", test_rx) // */
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
