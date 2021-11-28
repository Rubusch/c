/*
  cunit - tests
 */

#include "binary-search-tree.h"

#include "test.h"

#include <stdlib.h>
#include <time.h>
#include <stdint.h>


void test_tree_root(void)
{
	// empty
	node_p node = tree_root();
	CU_ASSERT(NULL == node);

	// set root
	char *value = malloc(sizeof(*value));
	if (!value) {
		perror("allocation failure");
		exit(EXIT_FAILURE);
	}
	*value = 'A';
	tree_insert(123, &value[0]);
	node = tree_root();
	CU_ASSERT(NULL != node);

	// delete
	char* value_ret = (char*) tree_delete(node);
	CU_ASSERT(*value == *value_ret);
	free(value);
}

void test_tree_print(void)
{
	node_p node = tree_root();
	CU_ASSERT(NULL == node);

	// set root
	int size = 15;
	char *values = malloc(size * sizeof(*values));
	if (!values) {
		perror("allocation failure");
		exit(EXIT_FAILURE);
	}
	values[0] = 'A';

	values[1] = 'B';
	values[2] = 'C';

	values[3] = 'D';
	values[4] = 'E';
	values[5] = 'F';
	values[6] = 'G';

	values[7] = 'H';
	values[8] = 'I';
	values[9] = 'J';
	values[10] = 'K';
	values[11] = 'L';
	values[12] = 'M';
	values[13] = 'N';
	values[14] = 'O';

	tree_insert(7, &values[7]);

	tree_insert(3, &values[3]);
	tree_insert(11, &values[11]);

	tree_insert(1, &values[1]);
	tree_insert(5, &values[5]);
	tree_insert(9, &values[9]);
	tree_insert(13, &values[13]);

	tree_insert(0, &values[0]);
	tree_insert(2, &values[2]);
	tree_insert(4, &values[4]);
	tree_insert(6, &values[6]);
	tree_insert(8, &values[8]);
	tree_insert(10, &values[10]);
	tree_insert(12, &values[12]);
	tree_insert(14, &values[14]);

	node = tree_root();
	CU_ASSERT('H' == *(char*) tree_get_data(node));
	tree_print_dot("tree.dot", node);

	// delete
	node = tree_root();
	for(node = tree_root(); node != NULL; node = tree_root()) {
		tree_delete(node);
	}
	free(values);
}

void test_tree_transplant(void)
{
	node_p node = tree_root();
	CU_ASSERT(NULL == node);

	// set root
	int size = 15;
	char *values = malloc(size * sizeof(*values));
	if (!values) {
		perror("allocation failure");
		exit(EXIT_FAILURE);
	}
	values[0] = 'A';

	values[1] = 'B';
	values[2] = 'C';

	values[3] = 'D';
	values[4] = 'E';
	values[5] = 'F';
	values[6] = 'G';

	values[7] = 'H';
	values[8] = 'I';
	values[9] = 'J';
	values[10] = 'K';
	values[11] = 'L';
	values[12] = 'M';
	values[13] = 'N';
	values[14] = 'O';

//*	// right
	tree_insert(7, &values[7]);
	tree_insert(11, &values[11]);
	tree_insert(9, &values[9]);
	tree_insert(13, &values[13]);
	node = tree_root();
	tree_print_dot("tree_transplant_root_right__before.dot", node);
	CU_ASSERT(7 == node->key);

       	{
		node = tree_root();
		tree_transplant(node, node->right);
	}

	node = tree_root();
	tree_print_dot("tree_transplant_root_right__after.dot", node);
	CU_ASSERT(11 == node->key);

	for(node = tree_root(); node != NULL; node = tree_root()) {
		tree_delete(node);
	}
// */

//*	// left
	tree_insert(7, &values[7]);
	tree_insert(3, &values[3]);
	tree_insert(1, &values[1]);
	tree_insert(5, &values[5]);
	node = tree_root();
	tree_print_dot("tree_transplant_root_left__before.dot", node);
	CU_ASSERT(7 == node->key);

       	{
		node = tree_root();
		tree_transplant(node, node->left);
	}

	node = tree_root();
	tree_print_dot("tree_transplant_root_left__after.dot", node);
	CU_ASSERT(3 == node->key);

	for(node = tree_root(); node != NULL; node = tree_root()) {
		tree_delete(node);
	}
// */


//*	// right subtree with valid root->left
	tree_insert(7, &values[7]);
	tree_insert(3, &values[3]);
	tree_insert(11, &values[11]);
	tree_insert(13, &values[13]);
	node = tree_root();
	tree_print_dot("tree_transplant_root_right_nil__before.dot", node);
	CU_ASSERT(7 == node->key);

	{
		node = tree_root();
		node_p tmp = tree_minimum(node->right);
		if (tmp->parent != node) {
			tree_transplant(tmp, tmp->right);
			tmp->right = node->right;
			tmp->right->parent = tmp;
		}
		tree_transplant(node, tmp);
		tmp->left = node->left;
		if (tmp->left) tmp->left->parent = tmp;
	}

	node = tree_root();
	tree_print_dot("tree_transplant_root_right_nil__after.dot", node);
	CU_ASSERT(11 == node->key);

	for(node = tree_root(); node != NULL; node = tree_root()) {
		tree_delete(node);
	}
// */


//*	// right, right, nil
	tree_insert(7, &values[7]);
	tree_insert(3, &values[3]);
	tree_insert(11, &values[11]);
	tree_insert(9, &values[9]);
	tree_insert(10, &values[10]);
	tree_insert(13, &values[13]);
	tree_insert(12, &values[12]);
	tree_insert(14, &values[14]);

	node = tree_root();
	tree_print_dot("tree_transplant_root_right_right_nil__before.dot", node);
	CU_ASSERT(7 == node->key);

	{
		node = tree_root();
		node_p tmp = tree_minimum(node->right);
		if (tmp->parent != node) {
			tree_transplant(tmp, tmp->right);
			tmp->right = node->right;
			tmp->right->parent = tmp;
		}
		tree_transplant(node, tmp);
		tmp->left = node->left;
		if (tmp->left) tmp->left->parent = tmp;
	}

	node = tree_root();
	tree_print_dot("tree_transplant_root_right_right_nil__after.dot", node);
	CU_ASSERT(9 == node->key);

	for(node = tree_root(); node != NULL; node = tree_root()) {
		tree_delete(node);
	}
// */

//*	// right, right, right, nil
	tree_insert(7, &values[7]);
	tree_insert(3, &values[3]);
	tree_insert(11, &values[11]);
	tree_insert(9, &values[9]);
	tree_insert(8, &values[8]);
	tree_insert(10, &values[10]);
	tree_insert(13, &values[13]);
	tree_insert(12, &values[12]);
	tree_insert(14, &values[14]);

	node = tree_root();
	tree_print_dot("tree_transplant_root_right_right_right_nil__before.dot", node);
	CU_ASSERT(7 == node->key);

	{
		node = tree_root();
		node_p tmp = tree_minimum(node->right);
		if (tmp->parent != node) {
			tree_transplant(tmp, tmp->right);
			tmp->right = node->right;
			tmp->right->parent = tmp;
		}
		tree_transplant(node, tmp);
		tmp->left = node->left;
		if (tmp->left) tmp->left->parent = tmp;
	}

	node = tree_root();
	tree_print_dot("tree_transplant_root_right_right_right_nil__after.dot", node);
	CU_ASSERT(8 == node->key);

	for(node = tree_root(); node != NULL; node = tree_root()) {
		tree_delete(node);
	}
// */

//*	// left, nil
	tree_insert(7, &values[7]);
	tree_insert(3, &values[3]);
	tree_insert(11, &values[11]);
	tree_insert(1, &values[1]);

	node = tree_root();
	tree_print_dot("tree_transplant_root_left_nil__before.dot", node);
	CU_ASSERT(7 == node->key);

	{
		node = tree_root();
		node_p tmp = tree_minimum(node->right);
		if (tmp->parent != node) {
			tree_transplant(tmp, tmp->right);
			tmp->right = node->right;
			tmp->right->parent = tmp;
		}
		tree_transplant(node, tmp);
		tmp->left = node->left;
		if (tmp->left) tmp->left->parent = tmp;
	}

	node = tree_root();
	tree_print_dot("tree_transplant_root_left_nil__after.dot", node);
	CU_ASSERT(11 == node->key);

	for(node = tree_root(); node != NULL; node = tree_root()) {
		tree_delete(node);
	}
// */

//*	// left, left, nil
	tree_insert(7, &values[7]);
	tree_insert(3, &values[3]);
	tree_insert(11, &values[11]);
	tree_insert(1, &values[1]);
	tree_insert(5, &values[5]);
	tree_insert(0, &values[0]);
	tree_insert(2, &values[2]);
	tree_insert(4, &values[4]);

	node = tree_root();
	tree_print_dot("tree_transplant_root_left_left_nil__before.dot", node);
	CU_ASSERT(7 == node->key);

	{
		node = tree_root();
		node_p tmp = tree_minimum(node->right);
		if (tmp->parent != node) {
			tree_transplant(tmp, tmp->right);
			tmp->right = node->right;
			tmp->right->parent = tmp;
		}
		tree_transplant(node, tmp);
		tmp->left = node->left;
		if (tmp->left) tmp->left->parent = tmp;
	}

	node = tree_root();
	tree_print_dot("tree_transplant_root_left_left_nil__after.dot", node);
	CU_ASSERT(11 == node->key);

	for(node = tree_root(); node != NULL; node = tree_root()) {
		tree_delete(node);
	}
// */

//*	// left, left, left, nil
	tree_insert(7, &values[7]);
	tree_insert(3, &values[3]);
	tree_insert(11, &values[11]);
	tree_insert(1, &values[1]);
	tree_insert(5, &values[5]);
	tree_insert(0, &values[0]);
	tree_insert(2, &values[2]);
	tree_insert(4, &values[4]);
	tree_insert(6, &values[6]);

	node = tree_root();
	tree_print_dot("tree_transplant_root_left_left_left_nil__before.dot", node);
	CU_ASSERT(7 == node->key);

	{
		node = tree_root();
		node_p tmp = tree_minimum(node->right);
		if (tmp->parent != node) {
			tree_transplant(tmp, tmp->right);
			tmp->right = node->right;
			tmp->right->parent = tmp;
		}
		tree_transplant(node, tmp);
		tmp->left = node->left;
		if (tmp->left) tmp->left->parent = tmp;
	}

	node = tree_root();
	tree_print_dot("tree_transplant_root_left_left_left_nil__after.dot", node);
	CU_ASSERT(11 == node->key);

	for(node = tree_root(); node != NULL; node = tree_root()) {
		tree_delete(node);
	}
// */

//*	// right, full
	tree_insert(7, &values[7]);
	tree_insert(3, &values[3]);
	tree_insert(11, &values[11]);
	tree_insert(9, &values[9]);
	tree_insert(8, &values[8]);
	tree_insert(10, &values[10]);
	tree_insert(13, &values[13]);
	tree_insert(12, &values[12]);
	tree_insert(14, &values[14]);

	node = tree_root();
	tree_print_dot("tree_transplant_root_right_full__before.dot", node);
	CU_ASSERT(7 == node->key);

	{
		node = tree_root();
		node_p tmp = tree_minimum(node->right);
		if (tmp->parent != node) {
			tree_transplant(tmp, tmp->right);
			tmp->right = node->right;
			tmp->right->parent = tmp;
		}
		tree_transplant(node, tmp);
		tmp->left = node->left;
		if (tmp->left) tmp->left->parent = tmp;
	}

	node = tree_root();
	tree_print_dot("tree_transplant_root_right_full__after.dot", node);
	CU_ASSERT(8 == node->key);

	for(node = tree_root(); node != NULL; node = tree_root()) {
		tree_delete(node);
	}
// */

	free(values);
}


/*
void test_tree_minimum()
{
	// TODO
}

void test_tree_maximum()
{
	// TODO
}

//*/
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
	TEST_start(pSuite, "tree root", test_tree_root)
 		TEST_append(pSuite, "tree print", test_tree_print) // */
 		TEST_append(pSuite, "tree transplant", test_tree_transplant) // */
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
