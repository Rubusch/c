/*
  cunit - tests
 */

#include "binary-search-tree.h"

#include "test.h"

#include <stdlib.h>
#include <time.h>
#include <stdint.h>

void test_tree_minimum()
{
	// TODO
}

void test_tree_maximum()
{
	// TODO
}

void test_tree_insert()
{
	// TODO
}

/*
void test_hash_key(void)
{
	uint64_t *key_a = malloc(sizeof(*key_a));
	if (!key_a) {
		perror("allocation failed");
		exit(EXIT_FAILURE);
	}

	// reliably produces hashes
	*key_a = 'L';
	uint64_t hash_a = hash_key(key_a);
	CU_ASSERT(hash_a = hash_key(key_a));
	CU_ASSERT(hash_a = hash_key(key_a));
	CU_ASSERT(hash_a = hash_key(key_a));
	CU_ASSERT(hash_a = hash_key(key_a));
	CU_ASSERT(hash_a = hash_key(key_a));

	uint64_t *key_b = malloc(sizeof(*key_b));
	if (!key_a) {
		perror("allocation failed");
		exit(EXIT_FAILURE);
	}

	// produces reproducible hashes
	*key_b = 'L';
	uint64_t hash_b = hash_key(key_a);
	CU_ASSERT(hash_a = hash_b);

	free(key_a);
	free(key_b);
}
// */


int main(void)
{
	CU_pSuite pSuite = NULL;

	/* initialize the CUnit test registry */
	if (CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();

	/* add a suite to the registry */
	pSuite = CU_add_suite("binary search tree", init_suite_binarysearchtree,
			      clean_suite_binarysearchtree);
	if (NULL == pSuite) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	/* utilities */
	TEST_start(pSuite, "hashing", test_hash_key)
/* 		TEST_append(pSuite, "size", test_hash_size) // */
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
