/*
  cunit - tests
 */

#include "hashtable.h"

#include "test.h"
#include "stdlib.h"
#include "time.h"


void test_hashing_insert(void)
{
	unsigned int arr[] = { 95, 15, 20, 65, 40, 55, 45, 30, 50, 70, 15 };
	int size = sizeof(arr) / sizeof(*arr);

	insert(arr, size);

	unsigned int x = 30;
	CU_ASSERT(search(x));
}


int main(void)
{
	CU_pSuite pSuite = NULL;

	/* initialize the CUnit test registry */
	if (CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();

	/* add a suite to the registry */
	pSuite = CU_add_suite("hashtable", init_suite_hashtable,
			      clean_suite_hashtable);
	if (NULL == pSuite) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	/* utilities */
	TEST_start(pSuite, "hashing", test_hashing_insert)
/* 		TEST_append(pSuite, "empty", test_list_empty) // */
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
