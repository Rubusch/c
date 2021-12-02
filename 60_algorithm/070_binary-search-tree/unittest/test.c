/*
  cunit - tests

  compile
  $ make clean && make basictest.exe

  test
  $ ./basictest.exe

  verification
  $ valgrind --track-origins=yes --leak-check=full ./basictest.exe

  ==30041== HEAP SUMMARY:
  ==30041==     in use at exit: 0 bytes in 0 blocks
  ==30041==   total heap usage: 386 allocs, 386 frees, 18,783 bytes allocated
  ==30041==
  ==30041== All heap blocks were freed -- no leaks are possible
  ==30041==
  ==30041== For counts of detected and suppressed errors, rerun with: -v
  ==30041== ERROR SUMMARY: 36696 errors from 256 contexts (suppressed: 464 from 1)

  graphical representation
  (via print function)
  $ dot -Tpng ./tree.dot -o tree.png && fim ./tree.png &

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
	tree_delete(&node);
	free(value);
}

void test_tree_get_data(void)
{
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
	char* value_ret = (char*) tree_delete(&node);
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

	// verification
	node = tree_root();
	CU_ASSERT('H' == *(char*) tree_get_data(node));
	tree_print_dot("tree.dot", node);

	// delete
	for (node = tree_root(); node != NULL; node = tree_root()) {
		tree_delete(&node);
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

	// right
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

		// delete
		free(node);
	}

	node = tree_root();
	tree_print_dot("tree_transplant_root_right__after.dot", node);
	CU_ASSERT(11 == node->key);

	// delete
	for (node = tree_root(); node != NULL; node = tree_root()) {
		tree_delete(&node);
	}

	// left
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

		// delete
		free(node);
	}

	node = tree_root();
	tree_print_dot("tree_transplant_root_left__after.dot", node);
	CU_ASSERT(3 == node->key);

	// delete
	for (node = tree_root(); node != NULL; node = tree_root()) {
		tree_delete(&node);
	}

	// right subtree with valid root->left
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

		// delete
		free(node);
	}

	node = tree_root();
	tree_print_dot("tree_transplant_root_right_nil__after.dot", node);
	CU_ASSERT(11 == node->key);

	// delete
	for (node = tree_root(); node != NULL; node = tree_root()) {
		tree_delete(&node);
	}

	// right, right, nil
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

		// delete
		free(node);
	}

	node = tree_root();
	tree_print_dot("tree_transplant_root_right_right_nil__after.dot", node);
	CU_ASSERT(9 == node->key);

	// delete
	for (node = tree_root(); node != NULL; node = tree_root()) {
		tree_delete(&node);
	}

	// right, right, right, nil
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

		// delete
		free(node);
	}

	node = tree_root();
	tree_print_dot("tree_transplant_root_right_right_right_nil__after.dot", node);
	CU_ASSERT(8 == node->key);

	// delete
	for (node = tree_root(); node != NULL; node = tree_root()) {
		tree_delete(&node);
	}

	// left, nil
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

		// delete
		free(node);
	}

	node = tree_root();
	tree_print_dot("tree_transplant_root_left_nil__after.dot", node);
	CU_ASSERT(11 == node->key);

	// delete
	for (node = tree_root(); node != NULL; node = tree_root()) {
		tree_delete(&node);
	}

	// left, left, nil
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

		// delete
		free(node);
	}

	node = tree_root();
	tree_print_dot("tree_transplant_root_left_left_nil__after.dot", node);
	CU_ASSERT(11 == node->key);

	// delete
	for (node = tree_root(); node != NULL; node = tree_root()) {
		tree_delete(&node);
	}

	// left, left, left, nil
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

		// delete
		free(node);
	}

	node = tree_root();
	tree_print_dot("tree_transplant_root_left_left_left_nil__after.dot", node);
	CU_ASSERT(11 == node->key);

	// delete
	for (node = tree_root(); node != NULL; node = tree_root()) {
		tree_delete(&node);
	}

	// right, full
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

		// delete
		free(node);
	}

	node = tree_root();
	tree_print_dot("tree_transplant_root_right_full__after.dot", node);
	CU_ASSERT(8 == node->key);

	// delete
	for (node = tree_root(); node != NULL; node = tree_root()) {
		tree_delete(&node);
	}

	free(values);
}

void test_tree_minimum(void)
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
	tree_print_dot("tree_minimum.dot", node);

	// verification
	node_p root = tree_root();
	node = tree_minimum(root);
	CU_ASSERT(0 == node->key);

	node = tree_minimum(root->left);
	CU_ASSERT(0 == node->key);

	node = tree_minimum(root->right);
	CU_ASSERT(8 == node->key);

	node = tree_minimum(root->right->right);
	CU_ASSERT(12 == node->key);

	node = tree_minimum(root->right->right->right);
	CU_ASSERT(14 == node->key);

	// delete
	for (node = tree_root(); node != NULL; node = tree_root()) {
		tree_delete(&node);
	}

	free(values);
}

void test_tree_maximum(void)
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
	tree_print_dot("tree_maximum.dot", node);

	// verification
	node_p root = tree_root();
	node = tree_maximum(root);
	CU_ASSERT(14 == node->key);

	node = tree_maximum(root->right);
	CU_ASSERT(14 == node->key);

	node = tree_maximum(root->left);
	CU_ASSERT(6 == node->key);

	node = tree_maximum(root->left->left);
	CU_ASSERT(2 == node->key);

	node = tree_maximum(root->left->left->left);
	CU_ASSERT(0 == node->key);

	// delete
	for (node = tree_root(); node != NULL; node = tree_root()) {
		tree_delete(&node);
	}

	free(values);
}

void test_tree_search(void)
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
	tree_print_dot("tree_maximum.dot", node);

	// verification
	node_p root = tree_root();

	for (int idx = 0; idx < size; idx++) {
		char* ptr = (char*) tree_search(root, idx);
		CU_ASSERT(*ptr == values[idx]);
	}

	// delete
	for (node = tree_root(); node != NULL; node = tree_root()) {
		tree_delete(&node);
	}

	free(values);
}

void test_tree_search_iterative(void)
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
	tree_print_dot("tree_maximum.dot", node);

	// verification
	node_p root = tree_root();

	for (int idx = 0; idx < size; idx++) {
		char* ptr = (char*) tree_search_iterative(root, idx);
		CU_ASSERT(*ptr == values[idx]);
	}

	// delete
	for (node = tree_root(); node != NULL; node = tree_root()) {
		tree_delete(&node);
	}

	free(values);
}

void test_tree_successor(void)
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
	tree_print_dot("tree_minimum.dot", node);

	// verification
	node_p root = tree_root();
	node = tree_minimum(root);
	CU_ASSERT(0 == node->key);

	for (int idx = 0; idx < size; idx++) {
		if (node) {
			CU_ASSERT(idx == node->key);
		} else {
			CU_ASSERT(FALSE);
		}
		node = tree_successor(node);
	}

	// delete
	for (node = tree_root(); node != NULL; node = tree_root()) {
		tree_delete(&node);
	}

	free(values);
}

void test_tree_predecessor(void)
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
	tree_print_dot("tree_minimum.dot", node);

	// verification
	node_p root = tree_root();
	node = tree_maximum(root);
	CU_ASSERT(14 == node->key);

	for (int idx = size -1; idx >= 0; idx--) {
		if (node) {
			CU_ASSERT(idx == node->key);
		} else {
			CU_ASSERT(FALSE);
		}
		node = tree_predecessor(node);
	}

	// delete
	for (node = tree_root(); node != NULL; node = tree_root()) {
		tree_delete(&node);
	}

	free(values);
}


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
 		TEST_append(pSuite, "tree get data", test_tree_get_data)
 		TEST_append(pSuite, "tree print", test_tree_print)
 		TEST_append(pSuite, "tree transplant", test_tree_transplant)
 		TEST_append(pSuite, "tree minimum", test_tree_minimum)
 		TEST_append(pSuite, "tree maximum", test_tree_maximum)
 		TEST_append(pSuite, "tree search", test_tree_search)
 		TEST_append(pSuite, "tree iterative search", test_tree_search_iterative)
 		TEST_append(pSuite, "tree successor", test_tree_successor)
 		TEST_append(pSuite, "tree predecessor", test_tree_predecessor)
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


	/* clean up registry and return */
	CU_cleanup_registry();
	return CU_get_error();
}
