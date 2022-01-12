/*
  cunit - tests
 */
#include <stdlib.h>

#include "greedy-huffman.h"

#include "test.h"


void test_huffman(void)
{
    char arr[] = { 'a', 'b', 'c', 'd', 'e', 'f' };
    int freq[] = { 5, 9, 12, 13, 16, 45 };
    int size = sizeof(arr) / sizeof(arr[0]);

    greedy_debug("\n");
    huffman_node_p root = NULL;
    huffman(&root, arr, freq, size);

    CU_ASSERT(root->right->right->left->left->data == 'a');
    CU_ASSERT(root->right->right->left->right->data == 'b');
    CU_ASSERT(root->right->left->left->data == 'c');
    CU_ASSERT(root->right->left->right->data == 'd');
    CU_ASSERT(root->right->right->right->data == 'e');
    CU_ASSERT(root->left->data == 'f');
}


int main(void)
{
	CU_pSuite pSuite = NULL;

	/* initialize the CUnit test registry */
	if (CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();

	/* add a suite to the registry */
	pSuite = CU_add_suite("greedy",
			      init_suite_greedy,
			      clean_suite_greedy);
	if (NULL == pSuite) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	/* utilities */
	TEST_start(pSuite, "huffman codes", test_huffman)
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
