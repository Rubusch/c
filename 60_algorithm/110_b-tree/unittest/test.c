/*
  cunit - tests
 */
#include <stdlib.h>

#include "b-tree.h"

#include "test.h"


void test_element(void)
{
	const int expected_key = 123;
	int key = expected_key;
	int *pdata = NULL;
	int *pdata_remain = NULL;
	const int expected_pdata = 7;
	element_p elem = NULL;

	pdata = malloc(sizeof(*pdata));
	if (!pdata) {
		btree_failure("%s [%d]: %s() - allocation failed",
			      __FILE__, __LINE__, __func__);
	}
	*pdata = expected_pdata;

	CU_ASSERT(NULL == pdata_remain);
	CU_ASSERT(NULL == elem);
	elem = create_key(pdata, key);
	CU_ASSERT(NULL != elem);
	if (!elem) {
		goto cleanup;
	}

	CU_ASSERT(expected_key == elem->val);
	CU_ASSERT(expected_pdata == *(int*) elem->data);

	pdata_remain = (int*) destroy_key(&elem);
	CU_ASSERT(NULL == elem);
	CU_ASSERT(expected_pdata == *pdata_remain);

cleanup:
	free(pdata);
	pdata = NULL;
}

void test_btree_create(void)
{
	CU_ASSERT(NULL == btree_root());
	btree_create();
	CU_ASSERT(NULL != btree_root());
	btree_destroy(btree_root());
	CU_ASSERT(NULL == btree_root());
}
/*
void test_btree_insert(void)
{
	CU_ASSERT(NULL == btree_root());
	btree_create();
	CU_ASSERT(NULL != btree_root());

	element_p key;
	key = create_key(NULL, 7);
	btree_insert(key);

	// TODO       
	btree_destroy(&btree_root());
	CU_ASSERT(NULL == btree_root());
}
// */


int main(void)
{
	CU_pSuite pSuite = NULL;

	/* initialize the CUnit test registry */
	if (CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();

	/* add a suite to the registry */
	pSuite = CU_add_suite("b-tree",
			      init_suite_btree,
			      clean_suite_btree);
	if (NULL == pSuite) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	/* utilities */
	TEST_start(pSuite, "element", test_element)
		TEST_append(pSuite, "b-tree-create", test_btree_create) // */
// 		TEST_append(pSuite, "b-tree-insert", test_btree_insert) // */
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
