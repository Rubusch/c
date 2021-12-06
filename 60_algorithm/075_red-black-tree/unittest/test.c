/*
  cunit - tests

  NB: red_black_delete_fixup() and red_black_insert_fixup() should be
  tested separately, i.e. insert and insert_fixup should be divided
  into separate test cases, as also delete and delete_fixup. Anyhow,
  this implies fabricating certain tree situations, and confirm a
  correct red-black re-balancing, especially in the fixup
  functions. As also covering all NULL cases. The current test just
  tests the entire insert-fixup and delete-fixup situation +/-
  sufficient for demo cases.


  compile
  $ make clean && make basictest.exe

  test
  $ ./basictest.exe

  verification
  $ valgrind --track-origins=yes --leak-check=full ./basictest.exe
    ...
    ==17766== HEAP SUMMARY:
    ==17766==     in use at exit: 0 bytes in 0 blocks
    ==17766==   total heap usage: 307 allocs, 307 frees, 202,320 bytes allocated
    ==17766==
    ==17766== All heap blocks were freed -- no leaks are possible

  graphical representation
  (via print function)
  $ dot -Tpng ./tree.dot -o tree.png && fim ./tree.png &

 */

#include "red-black-tree.h"

#include "test.h"

#include <stdlib.h>
#include <time.h>
#include <stdint.h>


void test_tree_root(void)
{
	// empty
	node_p node = tree_root();
	CU_ASSERT(NULL == node);
	if (NULL != node) return;

	// set root
	char *value = malloc(sizeof(*value));
	if (!value) {
		perror("allocation failure");
		exit(EXIT_FAILURE);
	}
	*value = 'A';
	red_black_insert(123, &value[0]);
	node = tree_root();
	CU_ASSERT(NULL != node);
	CU_ASSERT(123 == node->key);
	CU_ASSERT(BLACK == node->color);
	CU_ASSERT(NULL == node->parent);
	CU_ASSERT(NULL == node->left);
	CU_ASSERT(NULL == node->right);

	// delete
	red_black_delete(&node);
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
	red_black_insert(123, &value[0]);
	node = tree_root();
	CU_ASSERT(NULL != node);
	CU_ASSERT('A' == *(char*) tree_get_data(node));
	CU_ASSERT(NULL == node->parent);
	CU_ASSERT(NULL == node->left);
	CU_ASSERT(NULL == node->right);

	// delete
	char* value_ret = (char*) red_black_delete(&node);
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

	red_black_insert(7, &values[7]);

	red_black_insert(3, &values[3]);
	red_black_insert(11, &values[11]);

	red_black_insert(1, &values[1]);
	red_black_insert(5, &values[5]);
	red_black_insert(9, &values[9]);
	red_black_insert(13, &values[13]);

	red_black_insert(0, &values[0]);
	red_black_insert(2, &values[2]);
	red_black_insert(4, &values[4]);
	red_black_insert(6, &values[6]);
	red_black_insert(8, &values[8]);
	red_black_insert(10, &values[10]);
	red_black_insert(12, &values[12]);
	red_black_insert(14, &values[14]);

	// verification
	node = tree_root();
	CU_ASSERT(NULL == node->parent);

	CU_ASSERT( 7 == node->key);
	CU_ASSERT('H' == *(char*) tree_get_data(node));

	CU_ASSERT( 3 == node->left->key);
	CU_ASSERT('D' == *(char*) tree_get_data(node->left));
	CU_ASSERT(11 == node->right->key);
	CU_ASSERT('L' == *(char*) tree_get_data(node->right));

	CU_ASSERT( 1 == node->left->left->key);
	CU_ASSERT('B' == *(char*) tree_get_data(node->left->left));
	CU_ASSERT( 5 == node->left->right->key);
	CU_ASSERT('F' == *(char*) tree_get_data(node->left->right));
	CU_ASSERT( 9 == node->right->left->key);
	CU_ASSERT('J' == *(char*) tree_get_data(node->right->left));
	CU_ASSERT(13 == node->right->right->key);
	CU_ASSERT('N' == *(char*) tree_get_data(node->right->right));

	CU_ASSERT( 0 == node->left->left->left->key);
	CU_ASSERT('A' == *(char*) tree_get_data(node->left->left->left));
	CU_ASSERT( 2 == node->left->left->right->key);
	CU_ASSERT('C' == *(char*) tree_get_data(node->left->left->right));
	CU_ASSERT( 4 == node->left->right->left->key);
	CU_ASSERT('E' == *(char*) tree_get_data(node->left->right->left));
	CU_ASSERT( 6 == node->left->right->right->key);
	CU_ASSERT('G' == *(char*) tree_get_data(node->left->right->right));
	CU_ASSERT( 8 == node->right->left->left->key);
	CU_ASSERT('I' == *(char*) tree_get_data(node->right->left->left));
	CU_ASSERT(10 == node->right->left->right->key);
	CU_ASSERT('K' == *(char*) tree_get_data(node->right->left->right));
	CU_ASSERT(12 == node->right->right->left->key);
	CU_ASSERT('M' == *(char*) tree_get_data(node->right->right->left));
	CU_ASSERT(14 == node->right->right->right->key);
	CU_ASSERT('O' == *(char*) tree_get_data(node->right->right->right));

	tree_print_dot("tree.dot", node);

	// delete
//*
	int idx=0;
	for (node = tree_root(); node != NULL; node = tree_root()) {
//	for (int idx = 0; idx < 2; idx++) {
		char str[16];
		sprintf(str, "%s%d%s", "tree", idx, ".dot");
		idx++;
//		node = tree_root();
		tree_print_dot(str, node);
		red_black_delete(&node);
	}
/*/
	node = tree_root();
	tree_print_dot("tree0.dot", node);
	red_black_delete(&node);

	node = tree_root();
	tree_print_dot("tree1.dot", node);
	red_black_delete(&node);

	node = tree_root();
	tree_print_dot("tree2.dot", node);
//	red_black_delete(&node);
// */

	free(values);
}

void test_tree_left_rotate(void)
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

	red_black_insert(7, &values[7]);
	red_black_insert(3, &values[3]);
	red_black_insert(11, &values[11]);
	red_black_insert(9, &values[9]);

	// verification
	node = tree_root();
	CU_ASSERT(7 == node->key);
	CU_ASSERT(NULL == node->parent);
	CU_ASSERT(11 == node->right->key);
	CU_ASSERT(NULL == node->right->right);
	CU_ASSERT(3 == node->left->key);
	CU_ASSERT(NULL == node->left->left);
	CU_ASSERT(NULL == node->left->right);
	CU_ASSERT(9 == node->right->left->key);
	CU_ASSERT(NULL == node->right->left->left);
	CU_ASSERT(NULL == node->right->left->right);
	tree_print_dot("leftrotate0.dot", node);

	red_black_left_rotate(node);

	node = tree_root();
	CU_ASSERT(11 == node->key);
	CU_ASSERT(NULL == node->parent);
	CU_ASSERT(NULL == node->right);
	CU_ASSERT(7 == node->left->key);
	CU_ASSERT(3 == node->left->left->key);
	CU_ASSERT(NULL == node->left->left->left);
	CU_ASSERT(NULL == node->left->left->right);
	CU_ASSERT(9 == node->left->right->key);
	CU_ASSERT(NULL == node->left->right->left);
	CU_ASSERT(NULL == node->left->right->right);
	tree_print_dot("leftrotate1.dot", node);

	// delete
	for (node = tree_root(); node != NULL; node = tree_root()) {
		red_black_delete(&node);
	}
	free(values);
}

void test_tree_right_rotate(void)
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

	red_black_insert(7, &values[7]);
	red_black_insert(3, &values[3]);
	red_black_insert(11, &values[11]);
	red_black_insert(5, &values[9]);

	// verification
	node = tree_root();
	CU_ASSERT(7 == node->key);
	CU_ASSERT(NULL == node->parent);
	CU_ASSERT(3 == node->left->key);
	CU_ASSERT(NULL == node->left->left);
	CU_ASSERT(11 == node->right->key);
	CU_ASSERT(NULL == node->right->left);
	CU_ASSERT(NULL == node->right->right);
	CU_ASSERT(5 == node->left->right->key);
	CU_ASSERT(NULL == node->left->right->left);
	CU_ASSERT(NULL == node->left->right->right);
	tree_print_dot("rightrotate0.dot", node);

	red_black_right_rotate(node);

	node = tree_root();
	CU_ASSERT(3 == node->key);
	CU_ASSERT(NULL == node->parent);
	CU_ASSERT(NULL == node->left);
	CU_ASSERT(7 == node->right->key);
	CU_ASSERT(11 == node->right->right->key);
	CU_ASSERT(NULL == node->right->right->left);
	CU_ASSERT(NULL == node->right->right->right);
	CU_ASSERT(5 == node->right->left->key);
	CU_ASSERT(NULL == node->right->left->left);
	CU_ASSERT(NULL == node->right->left->right);
	tree_print_dot("rightrotate1.dot", node);

	// delete
	for (node = tree_root(); node != NULL; node = tree_root()) {
		red_black_delete(&node);
	}
	free(values);
}

void test_red_black_insert_fixup(void)
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

	{
		red_black_insert(7, &values[7]);
		red_black_insert(3, &values[3]);
		red_black_insert(11, &values[11]);

		node = tree_root();

		CU_ASSERT( 7 == node->key);
		CU_ASSERT(BLACK == node->color);
		CU_ASSERT(NULL == node->parent);

		CU_ASSERT( 3 == node->left->key);
		CU_ASSERT(RED == node->left->color);
		CU_ASSERT(NULL == node->left->left);
		CU_ASSERT(NULL == node->left->right);

		CU_ASSERT(11 == node->right->key);
		CU_ASSERT(RED == node->right->color);
		CU_ASSERT(NULL == node->right->left);
		CU_ASSERT(NULL == node->right->right);

		red_black_insert(1, &values[1]);
		node = tree_root();

		CU_ASSERT( 7 == node->key);
		CU_ASSERT(BLACK == node->color);
		CU_ASSERT(NULL == node->parent);

 		CU_ASSERT( 3 == node->left->key);
		CU_ASSERT(BLACK == node->left->color);
		CU_ASSERT(NULL == node->left->right);

		CU_ASSERT(11 == node->right->key);
		CU_ASSERT(BLACK == node->right->color);
		CU_ASSERT(NULL == node->right->left);
		CU_ASSERT(NULL == node->right->right);

		CU_ASSERT(1 == node->left->left->key);
		CU_ASSERT(RED == node->left->left->color);
		CU_ASSERT(NULL == node->left->left->left);
		CU_ASSERT(NULL == node->left->left->right);

		red_black_insert(5, &values[5]);
		node = tree_root();

		CU_ASSERT(5 == node->left->right->key);
		CU_ASSERT(RED == node->left->right->color);
		CU_ASSERT(NULL == node->left->right->left);
		CU_ASSERT(NULL == node->left->right->right);

		red_black_insert(9, &values[9]);
		node = tree_root();

		CU_ASSERT(9 == node->right->left->key);
		CU_ASSERT(RED == node->right->left->color);
		CU_ASSERT(NULL == node->right->left->left);
		CU_ASSERT(NULL == node->right->left->right);

		red_black_insert(13, &values[13]);
		node = tree_root();

		CU_ASSERT(13 == node->right->right->key);
		CU_ASSERT(RED == node->right->right->color);
		CU_ASSERT(NULL == node->right->right->left);
		CU_ASSERT(NULL == node->right->right->right);

		red_black_insert(0, &values[0]);
		node = tree_root();

 		CU_ASSERT( 3 == node->left->key);
		CU_ASSERT(RED == node->left->color);

		CU_ASSERT( 1 == node->left->left->key);
		CU_ASSERT(BLACK == node->left->left->color);
		CU_ASSERT(NULL == node->left->left->right);

		CU_ASSERT(5 == node->left->right->key);
		CU_ASSERT(BLACK == node->left->right->color);
		CU_ASSERT(NULL == node->left->right->left);
		CU_ASSERT(NULL == node->left->right->right);

		CU_ASSERT( 0 == node->left->left->left->key);
		CU_ASSERT(RED == node->left->left->left->color);
		CU_ASSERT(NULL == node->left->left->left->left);
		CU_ASSERT(NULL == node->left->left->left->right);

		// delete
		for (node = tree_root(); node != NULL; node = tree_root()) {
			red_black_delete(&node);
		}
	}

	{
		red_black_insert(7, &values[7]);
		red_black_insert(11, &values[11]);
		red_black_insert(9, &values[9]); // provokes left rotation

		node = tree_root();

		CU_ASSERT( 9 == node->key);
		CU_ASSERT(BLACK == node->color);
		CU_ASSERT(NULL == node->parent);
		CU_ASSERT( 7 == node->left->key);
		CU_ASSERT(RED == node->left->color);
		CU_ASSERT(NULL == node->left->left);
		CU_ASSERT(NULL == node->left->right);
		CU_ASSERT(11 == node->right->key);
		CU_ASSERT(RED == node->right->color);
		CU_ASSERT(NULL == node->right->left);
		CU_ASSERT(NULL == node->right->right);

		// delete
		for (node = tree_root(); node != NULL; node = tree_root()) {
			red_black_delete(&node);
		}
	}

	{
		red_black_insert(7, &values[7]);
		red_black_insert(11, &values[11]);
		red_black_insert(9, &values[9]);
		red_black_insert(13, &values[13]); // colors all upper nodes BLACK

		node = tree_root();

		CU_ASSERT( 9 == node->key);
		CU_ASSERT(BLACK == node->color);
		CU_ASSERT(NULL == node->parent);
		CU_ASSERT( 7 == node->left->key);
		CU_ASSERT(BLACK == node->left->color);
		CU_ASSERT(NULL == node->left->left);
		CU_ASSERT(NULL == node->left->right);
		CU_ASSERT(11 == node->right->key);
		CU_ASSERT(BLACK == node->right->color);
		CU_ASSERT(NULL == node->right->left);
		CU_ASSERT(13 == node->right->right->key);
		CU_ASSERT(RED == node->right->right->color);

		// delete
		for (node = tree_root(); node != NULL; node = tree_root()) {
			red_black_delete(&node);
		}
	}

	{
		red_black_insert(7, &values[7]);
		red_black_insert(11, &values[11]);
		red_black_insert(9, &values[9]);
		red_black_insert(13, &values[13]);
		red_black_insert(14, &values[14]); // re-balancing

		node = tree_root();

		CU_ASSERT( 9 == node->key);
		CU_ASSERT(BLACK == node->color);
		CU_ASSERT(NULL == node->parent);
		CU_ASSERT( 7 == node->left->key);
		CU_ASSERT(BLACK == node->left->color);
		CU_ASSERT(NULL == node->left->left);
		CU_ASSERT(NULL == node->left->right);
		CU_ASSERT(13 == node->right->key);
		CU_ASSERT(BLACK == node->right->color);
		CU_ASSERT(11 == node->right->left->key);
		CU_ASSERT(RED == node->right->left->color);
		CU_ASSERT(NULL == node->right->left->left);
		CU_ASSERT(NULL == node->right->left->right);
		CU_ASSERT(14 == node->right->right->key);
		CU_ASSERT(RED == node->right->right->color);
		CU_ASSERT(NULL == node->right->right->left);
		CU_ASSERT(NULL == node->right->right->right);

		// delete
		for (node = tree_root(); node != NULL; node = tree_root()) {
			red_black_delete(&node);
		}
	}

	// delete
	for (node = tree_root(); node != NULL; node = tree_root()) {
		red_black_delete(&node);
	}
	free(values);
}

void test_red_black_delete_fixup(void)
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

	{
		red_black_insert(7, &values[7]);
		red_black_insert(3, &values[3]);
		red_black_insert(11, &values[11]);
		red_black_insert(1, &values[1]);
		red_black_insert(13, &values[13]);

		node = tree_root();

		CU_ASSERT( 7 == node->key);
		CU_ASSERT(NULL == node->parent);
		CU_ASSERT(BLACK == node->color);
		CU_ASSERT( 3 == node->left->key);
		CU_ASSERT(BLACK == node->left->color);
		CU_ASSERT(NULL == node->left->right);
		CU_ASSERT(11 == node->right->key);
		CU_ASSERT(BLACK == node->right->color);
		CU_ASSERT(NULL == node->right->left);
		CU_ASSERT( 1 == node->left->left->key);
		CU_ASSERT(RED == node->left->left->color);
		CU_ASSERT(NULL == node->left->left->left);
		CU_ASSERT(NULL == node->left->left->right);
		CU_ASSERT(13 == node->right->right->key);
		CU_ASSERT(RED == node->right->right->color);
		CU_ASSERT(NULL == node->right->right->left);
		CU_ASSERT(NULL == node->right->right->right);

		red_black_delete(&node->left->left);
		node = tree_root();

		CU_ASSERT( 7 == node->key);
		CU_ASSERT(NULL == node->parent);
		CU_ASSERT(BLACK == node->color);
		CU_ASSERT( 3 == node->left->key);
		CU_ASSERT(BLACK == node->left->color);
		CU_ASSERT(NULL == node->left->left);
		CU_ASSERT(NULL == node->left->right);
		CU_ASSERT(11 == node->right->key);
		CU_ASSERT(BLACK == node->right->color);
		CU_ASSERT(NULL == node->right->left);
		CU_ASSERT(13 == node->right->right->key);
		CU_ASSERT(RED == node->right->right->color);
		CU_ASSERT(NULL == node->right->right->left);
		CU_ASSERT(NULL == node->right->right->right);

		red_black_delete(&node->left);
		node = tree_root();

		CU_ASSERT(11 == node->key);
		CU_ASSERT(NULL == node->parent);
		CU_ASSERT(BLACK == node->color);
		CU_ASSERT( 7 == node->left->key);
		CU_ASSERT(BLACK == node->left->color);
		CU_ASSERT(NULL == node->left->left);
		CU_ASSERT(NULL == node->left->right);
		CU_ASSERT(13 == node->right->key);
		CU_ASSERT(BLACK == node->right->color);
		CU_ASSERT(NULL == node->right->left);
		CU_ASSERT(NULL == node->right->right);
// */
		// delete
		for (node = tree_root(); node != NULL; node = tree_root()) {
			red_black_delete(&node);
		}
	}

	{
		red_black_insert(7, &values[7]);
		red_black_insert(3, &values[3]);
		red_black_insert(11, &values[11]);
		red_black_insert(1, &values[1]);
		red_black_insert(13, &values[13]);

		node = tree_root();

		CU_ASSERT( 7 == node->key);
		CU_ASSERT(NULL == node->parent);
		CU_ASSERT(BLACK == node->color);
		CU_ASSERT( 3 == node->left->key);
		CU_ASSERT(BLACK == node->left->color);
		CU_ASSERT(NULL == node->left->right);
		CU_ASSERT(11 == node->right->key);
		CU_ASSERT(BLACK == node->right->color);
		CU_ASSERT(NULL == node->right->left);
		CU_ASSERT( 1 == node->left->left->key);
		CU_ASSERT(RED == node->left->left->color);
		CU_ASSERT(NULL == node->left->left->left);
		CU_ASSERT(NULL == node->left->left->right);
		CU_ASSERT(13 == node->right->right->key);
		CU_ASSERT(RED == node->right->right->color);
		CU_ASSERT(NULL == node->right->right->left);
		CU_ASSERT(NULL == node->right->right->right);

		red_black_delete(&node);
		node = tree_root();

		CU_ASSERT(11 == node->key);
		CU_ASSERT(BLACK == node->color);
		CU_ASSERT( 3 == node->left->key);
		CU_ASSERT(BLACK == node->left->color);
		CU_ASSERT(NULL == node->left->right);
		CU_ASSERT( 1 == node->left->left->key);
		CU_ASSERT(RED == node->left->left->color);
		CU_ASSERT(NULL == node->left->left->left);
		CU_ASSERT(NULL == node->left->left->right);
		CU_ASSERT(13 == node->right->key);
		CU_ASSERT(BLACK == node->right->color);
		CU_ASSERT(NULL == node->right->left);
		CU_ASSERT(NULL == node->right->right);

		red_black_delete(&node);
		node = tree_root();

		CU_ASSERT( 3 == node->key);
		CU_ASSERT(BLACK == node->color);
		CU_ASSERT( 1 == node->left->key);
		CU_ASSERT(BLACK == node->left->color);
		CU_ASSERT(NULL == node->left->left);
		CU_ASSERT(NULL == node->left->right);
		CU_ASSERT(13 == node->right->key);
		CU_ASSERT(BLACK == node->right->color);
		CU_ASSERT(NULL == node->right->left);
		CU_ASSERT(NULL == node->right->right);

		// delete
		for (node = tree_root(); node != NULL; node = tree_root()) {
			red_black_delete(&node);
		}
	}

	{
		red_black_insert(7, &values[7]);
		red_black_insert(3, &values[3]);
		red_black_insert(11, &values[11]);
		red_black_insert(9, &values[9]);
		red_black_insert(8, &values[8]);
		red_black_insert(10, &values[10]);

		node = tree_root();

		CU_ASSERT( 7 == node->key);
		CU_ASSERT(NULL == node->parent);
		CU_ASSERT(BLACK == node->color);
		CU_ASSERT( 3 == node->left->key);
		CU_ASSERT(BLACK == node->left->color);
		CU_ASSERT(NULL == node->left->left);
		CU_ASSERT(NULL == node->left->right);
		CU_ASSERT( 9 == node->right->key);
		CU_ASSERT(RED == node->right->color);
		CU_ASSERT( 8 == node->right->left->key);
		CU_ASSERT(BLACK == node->right->left->color);
		CU_ASSERT(NULL == node->right->left->left);
		CU_ASSERT(NULL == node->right->left->right);
		CU_ASSERT(11 == node->right->right->key);
		CU_ASSERT(BLACK == node->right->right->color);
		CU_ASSERT(NULL == node->right->right->right);
		CU_ASSERT(10 == node->right->right->left->key);
		CU_ASSERT(RED == node->right->right->left->color);
		CU_ASSERT(NULL == node->right->right->left->left);
		CU_ASSERT(NULL == node->right->right->left->right);

		red_black_delete(&node->left);
		node = tree_root();

		CU_ASSERT( 9 == node->key);
		CU_ASSERT(NULL == node->parent);
		CU_ASSERT(BLACK == node->color);
		CU_ASSERT( 7 == node->left->key);
		CU_ASSERT(BLACK == node->left->color);
		CU_ASSERT(NULL == node->left->left);
		CU_ASSERT(11 == node->right->key);
		CU_ASSERT(BLACK == node->right->color);
		CU_ASSERT(NULL == node->right->right);
		CU_ASSERT( 8 == node->left->right->key);
		CU_ASSERT(RED == node->left->right->color);
		CU_ASSERT(NULL == node->left->right->left);
		CU_ASSERT(NULL == node->left->right->right);
		CU_ASSERT(10 == node->right->left->key);
		CU_ASSERT(RED == node->right->left->color);
		CU_ASSERT(NULL == node->right->left->left);
		CU_ASSERT(NULL == node->right->left->right);

		// delete
		for (node = tree_root(); node != NULL; node = tree_root()) {
			red_black_delete(&node);
		}
	}

	{
		red_black_insert(7, &values[7]);

		red_black_insert(3, &values[3]);
		red_black_insert(11, &values[11]);

		red_black_insert(1, &values[1]);
		red_black_insert(5, &values[5]);
		red_black_insert(9, &values[9]);
		red_black_insert(13, &values[13]);
		red_black_insert(2, &values[2]);
		red_black_insert(4, &values[4]);
		red_black_insert(6, &values[6]);
		red_black_insert(8, &values[8]);
		red_black_insert(10, &values[10]);
		red_black_insert(12, &values[12]);
		red_black_insert(14, &values[14]);

		node = tree_root();
		tree_print_dot("delete00.dot", node);

		CU_ASSERT( 7 == node->key);
		CU_ASSERT(NULL == node->parent);
		CU_ASSERT(BLACK == node->color);
		CU_ASSERT( 3 == node->left->key);
		CU_ASSERT(RED == node->left->color);
		CU_ASSERT(11 == node->right->key);
		CU_ASSERT(RED == node->right->color);
		CU_ASSERT( 1 == node->left->left->key);
		CU_ASSERT(BLACK == node->left->left->color);
		CU_ASSERT(NULL == node->left->left->left); // node 0 not set
		CU_ASSERT( 5 == node->left->right->key);
		CU_ASSERT(BLACK == node->left->right->color);
		CU_ASSERT( 9 == node->right->left->key);
		CU_ASSERT(BLACK == node->right->left->color);
		CU_ASSERT(13 == node->right->right->key);
		CU_ASSERT(BLACK == node->right->right->color);
		CU_ASSERT( 2 == node->left->left->right->key);
		CU_ASSERT(RED == node->left->left->right->color);
		CU_ASSERT(NULL == node->left->left->right->left);
		CU_ASSERT(NULL == node->left->left->right->right);
		CU_ASSERT( 4 == node->left->right->left->key);
		CU_ASSERT(RED == node->left->right->left->color);
		CU_ASSERT(NULL == node->left->right->left->left);
		CU_ASSERT(NULL == node->left->right->left->right);
		CU_ASSERT( 6 == node->left->right->right->key);
		CU_ASSERT(RED == node->left->right->right->color);
		CU_ASSERT(NULL == node->left->right->right->left);
		CU_ASSERT(NULL == node->left->right->right->right);
		CU_ASSERT( 8 == node->right->left->left->key);
		CU_ASSERT(RED == node->right->left->left->color);
		CU_ASSERT(NULL == node->right->left->left->left);
		CU_ASSERT(NULL == node->right->left->left->right);
		CU_ASSERT(10 == node->right->left->right->key);
		CU_ASSERT(RED == node->right->left->right->color);
		CU_ASSERT(NULL == node->right->left->right->left);
		CU_ASSERT(NULL == node->right->left->right->right);
		CU_ASSERT(12 == node->right->right->left->key);
		CU_ASSERT(RED == node->right->right->left->color);
		CU_ASSERT(NULL == node->right->right->left->left);
		CU_ASSERT(NULL == node->right->right->left->right);
		CU_ASSERT(14 == node->right->right->right->key);
		CU_ASSERT(RED == node->right->right->right->color);
		CU_ASSERT(NULL == node->right->right->right->left);
		CU_ASSERT(NULL == node->right->right->right->right);

		// 1. delete 7
		red_black_delete(&node);
		node = tree_root();
		tree_print_dot("delete01.dot", node);

		CU_ASSERT( 8 == node->key);
		CU_ASSERT(NULL == node->parent);
		CU_ASSERT(BLACK == node->color);
		CU_ASSERT( 3 == node->left->key);
		CU_ASSERT(RED == node->left->color);
		CU_ASSERT(11 == node->right->key);
		CU_ASSERT(RED == node->right->color);
		CU_ASSERT( 1 == node->left->left->key);
		CU_ASSERT(BLACK == node->left->left->color);
		CU_ASSERT(NULL == node->left->left->left); // node 0 not set
		CU_ASSERT( 5 == node->left->right->key);
		CU_ASSERT(BLACK == node->left->right->color);
		CU_ASSERT( 9 == node->right->left->key);
		CU_ASSERT(BLACK == node->right->left->color);
		CU_ASSERT(NULL == node->right->left->left);
		CU_ASSERT(13 == node->right->right->key);
		CU_ASSERT(BLACK == node->right->right->color);
		CU_ASSERT( 2 == node->left->left->right->key);
		CU_ASSERT(RED == node->left->left->right->color);
		CU_ASSERT(NULL == node->left->left->right->left);
		CU_ASSERT(NULL == node->left->left->right->right);
		CU_ASSERT( 4 == node->left->right->left->key);
		CU_ASSERT(RED == node->left->right->left->color);
		CU_ASSERT(NULL == node->left->right->left->left);
		CU_ASSERT(NULL == node->left->right->left->right);
		CU_ASSERT( 6 == node->left->right->right->key);
		CU_ASSERT(RED == node->left->right->right->color);
		CU_ASSERT(NULL == node->left->right->right->left);
		CU_ASSERT(NULL == node->left->right->right->right);
		CU_ASSERT(10 == node->right->left->right->key);
		CU_ASSERT(RED == node->right->left->right->color);
		CU_ASSERT(NULL == node->right->left->right->left);
		CU_ASSERT(NULL == node->right->left->right->right);
		CU_ASSERT(12 == node->right->right->left->key);
		CU_ASSERT(RED == node->right->right->left->color);
		CU_ASSERT(NULL == node->right->right->left->left);
		CU_ASSERT(NULL == node->right->right->left->right);
		CU_ASSERT(14 == node->right->right->right->key);
		CU_ASSERT(RED == node->right->right->right->color);
		CU_ASSERT(NULL == node->right->right->right->left);
		CU_ASSERT(NULL == node->right->right->right->right);

		// 2. delete 8
		red_black_delete(&node);
		node = tree_root();
		tree_print_dot("delete02.dot", node);

		CU_ASSERT( 9 == node->key);
		CU_ASSERT(NULL == node->parent);
		CU_ASSERT(BLACK == node->color);
		CU_ASSERT( 3 == node->left->key);
		CU_ASSERT(RED == node->left->color);
		CU_ASSERT(11 == node->right->key);
		CU_ASSERT(RED == node->right->color);
		CU_ASSERT( 1 == node->left->left->key);
		CU_ASSERT(BLACK == node->left->left->color);
		CU_ASSERT(NULL == node->left->left->left); // node 0 not set
		CU_ASSERT( 5 == node->left->right->key);
		CU_ASSERT(BLACK == node->left->right->color);
		CU_ASSERT(10 == node->right->left->key);
		CU_ASSERT(BLACK == node->right->left->color);
		CU_ASSERT(NULL == node->right->left->left);
		CU_ASSERT(NULL == node->right->left->right);
		CU_ASSERT(13 == node->right->right->key);
		CU_ASSERT(BLACK == node->right->right->color);
		CU_ASSERT( 2 == node->left->left->right->key);
		CU_ASSERT(RED == node->left->left->right->color);
		CU_ASSERT(NULL == node->left->left->right->left);
		CU_ASSERT(NULL == node->left->left->right->right);
		CU_ASSERT( 4 == node->left->right->left->key);
		CU_ASSERT(RED == node->left->right->left->color);
		CU_ASSERT(NULL == node->left->right->left->left);
		CU_ASSERT(NULL == node->left->right->left->right);
		CU_ASSERT( 6 == node->left->right->right->key);
		CU_ASSERT(RED == node->left->right->right->color);
		CU_ASSERT(NULL == node->left->right->right->left);
		CU_ASSERT(NULL == node->left->right->right->right);
		CU_ASSERT(12 == node->right->right->left->key);
		CU_ASSERT(RED == node->right->right->left->color);
		CU_ASSERT(NULL == node->right->right->left->left);
		CU_ASSERT(NULL == node->right->right->left->right);
		CU_ASSERT(14 == node->right->right->right->key);
		CU_ASSERT(RED == node->right->right->right->color);
		CU_ASSERT(NULL == node->right->right->right->left);
		CU_ASSERT(NULL == node->right->right->right->right);

		// 3. delete 9
		red_black_delete(&node);
		node = tree_root();
		tree_print_dot("delete03.dot", node);

		CU_ASSERT(10 == node->key);
		CU_ASSERT(NULL == node->parent);
		CU_ASSERT(BLACK == node->color);
		CU_ASSERT( 3 == node->left->key);
		CU_ASSERT(RED == node->left->color);
		CU_ASSERT(13 == node->right->key);
		CU_ASSERT(RED == node->right->color);
		CU_ASSERT( 1 == node->left->left->key);
		CU_ASSERT(BLACK == node->left->left->color);
		CU_ASSERT(NULL == node->left->left->left); // node 0 not set
		CU_ASSERT( 5 == node->left->right->key);
		CU_ASSERT(BLACK == node->left->right->color);
		CU_ASSERT(11 == node->right->left->key);
		CU_ASSERT(BLACK == node->right->left->color);
		CU_ASSERT(NULL == node->right->left->left);
		CU_ASSERT( 2 == node->left->left->right->key);
		CU_ASSERT(RED == node->left->left->right->color);
		CU_ASSERT(NULL == node->left->left->right->left);
		CU_ASSERT(NULL == node->left->left->right->right);
		CU_ASSERT( 4 == node->left->right->left->key);
		CU_ASSERT(RED == node->left->right->left->color);
		CU_ASSERT(NULL == node->left->right->left->left);
		CU_ASSERT(NULL == node->left->right->left->right);
		CU_ASSERT( 6 == node->left->right->right->key);
		CU_ASSERT(RED == node->left->right->right->color);
		CU_ASSERT(NULL == node->left->right->right->left);
		CU_ASSERT(NULL == node->left->right->right->right);
		CU_ASSERT(12 == node->right->left->right->key);
		CU_ASSERT(RED == node->right->left->right->color);
		CU_ASSERT(NULL == node->right->left->right->left);
		CU_ASSERT(NULL == node->right->left->right->right);
		CU_ASSERT(14 == node->right->right->key);
		CU_ASSERT(BLACK == node->right->right->color);
		CU_ASSERT(NULL == node->right->right->left);
		CU_ASSERT(NULL == node->right->right->right);

		// 4. delete 10
		red_black_delete(&node);
		node = tree_root();
		tree_print_dot("delete04.dot", node);

		CU_ASSERT(11 == node->key);
		CU_ASSERT(NULL == node->parent);
		CU_ASSERT(BLACK == node->color);
		CU_ASSERT( 3 == node->left->key);
		CU_ASSERT(RED == node->left->color);
		CU_ASSERT(13 == node->right->key);
		CU_ASSERT(RED == node->right->color);
		CU_ASSERT( 1 == node->left->left->key);
		CU_ASSERT(BLACK == node->left->left->color);
		CU_ASSERT(NULL == node->left->left->left); // node 0 not set
		CU_ASSERT( 5 == node->left->right->key);
		CU_ASSERT(BLACK == node->left->right->color);
		CU_ASSERT(12 == node->right->left->key);
		CU_ASSERT(BLACK == node->right->left->color);
		CU_ASSERT(NULL == node->right->left->left);
		CU_ASSERT(NULL == node->right->left->right);
		CU_ASSERT( 2 == node->left->left->right->key);
		CU_ASSERT(RED == node->left->left->right->color);
		CU_ASSERT(NULL == node->left->left->right->left);
		CU_ASSERT(NULL == node->left->left->right->right);
		CU_ASSERT( 4 == node->left->right->left->key);
		CU_ASSERT(RED == node->left->right->left->color);
		CU_ASSERT(NULL == node->left->right->left->left);
		CU_ASSERT(NULL == node->left->right->left->right);
		CU_ASSERT( 6 == node->left->right->right->key);
		CU_ASSERT(RED == node->left->right->right->color);
		CU_ASSERT(NULL == node->left->right->right->left);
		CU_ASSERT(NULL == node->left->right->right->right);
		CU_ASSERT(14 == node->right->right->key);
		CU_ASSERT(BLACK == node->right->right->color);
		CU_ASSERT(NULL == node->right->right->left);
		CU_ASSERT(NULL == node->right->right->right);

		// 5. delete 11
		red_black_delete(&node);
		node = tree_root();
		tree_print_dot("delete05.dot", node);

		CU_ASSERT(12 == node->key);
		CU_ASSERT(NULL == node->parent);
		CU_ASSERT(BLACK == node->color);
		CU_ASSERT( 3 == node->left->key);
		CU_ASSERT(RED == node->left->color);
		CU_ASSERT(13 == node->right->key);
		CU_ASSERT(BLACK == node->right->color);
		CU_ASSERT(NULL == node->right->left);
		CU_ASSERT( 1 == node->left->left->key);
		CU_ASSERT(BLACK == node->left->left->color);
		CU_ASSERT(NULL == node->left->left->left); // node 0 not set
		CU_ASSERT( 5 == node->left->right->key);
		CU_ASSERT(BLACK == node->left->right->color);
		CU_ASSERT( 2 == node->left->left->right->key);
		CU_ASSERT(RED == node->left->left->right->color);
		CU_ASSERT(NULL == node->left->left->right->left);
		CU_ASSERT(NULL == node->left->left->right->right);
		CU_ASSERT( 4 == node->left->right->left->key);
		CU_ASSERT(RED == node->left->right->left->color);
		CU_ASSERT(NULL == node->left->right->left->left);
		CU_ASSERT(NULL == node->left->right->left->right);
		CU_ASSERT( 6 == node->left->right->right->key);
		CU_ASSERT(RED == node->left->right->right->color);
		CU_ASSERT(NULL == node->left->right->right->left);
		CU_ASSERT(NULL == node->left->right->right->right);
		CU_ASSERT(14 == node->right->right->key);
		CU_ASSERT(RED == node->right->right->color);
		CU_ASSERT(NULL == node->right->right->left);
		CU_ASSERT(NULL == node->right->right->right);

		// 6. delete 12
		red_black_delete(&node);
		node = tree_root();
		tree_print_dot("delete06.dot", node);

		CU_ASSERT(13 == node->key);
		CU_ASSERT(NULL == node->parent);
		CU_ASSERT(BLACK == node->color);
		CU_ASSERT( 3 == node->left->key);
		CU_ASSERT(RED == node->left->color);
		CU_ASSERT(14 == node->right->key);
		CU_ASSERT(BLACK == node->right->color);
		CU_ASSERT(NULL == node->right->left);
		CU_ASSERT(NULL == node->right->right);
		CU_ASSERT( 1 == node->left->left->key);
		CU_ASSERT(BLACK == node->left->left->color);
		CU_ASSERT(NULL == node->left->left->left); // node 0 not set
		CU_ASSERT( 5 == node->left->right->key);
		CU_ASSERT(BLACK == node->left->right->color);
		CU_ASSERT( 2 == node->left->left->right->key);
		CU_ASSERT(RED == node->left->left->right->color);
		CU_ASSERT(NULL == node->left->left->right->left);
		CU_ASSERT(NULL == node->left->left->right->right);
		CU_ASSERT( 4 == node->left->right->left->key);
		CU_ASSERT(RED == node->left->right->left->color);
		CU_ASSERT(NULL == node->left->right->left->left);
		CU_ASSERT(NULL == node->left->right->left->right);
		CU_ASSERT( 6 == node->left->right->right->key);
		CU_ASSERT(RED == node->left->right->right->color);
		CU_ASSERT(NULL == node->left->right->right->left);
		CU_ASSERT(NULL == node->left->right->right->right);

		// 7. delete 13
		red_black_delete(&node);
		node = tree_root();
		tree_print_dot("delete07.dot", node);

		CU_ASSERT(3 == node->key);
		CU_ASSERT(NULL == node->parent);
		CU_ASSERT(BLACK == node->color);
		CU_ASSERT( 1 == node->left->key);
		CU_ASSERT(BLACK == node->left->color);
		CU_ASSERT(NULL == node->left->left);
		CU_ASSERT( 5 == node->right->key);
		CU_ASSERT(RED == node->right->color);
		CU_ASSERT( 2 == node->left->right->key);
		CU_ASSERT(RED == node->left->right->color);
		CU_ASSERT(NULL == node->left->right->left);
		CU_ASSERT(NULL == node->left->right->right);
		CU_ASSERT( 4 == node->right->left->key);
		CU_ASSERT(BLACK == node->right->left->color);
		CU_ASSERT(NULL == node->right->left->left);
		CU_ASSERT(NULL == node->right->left->right);
		CU_ASSERT(14 == node->right->right->key);
		CU_ASSERT(BLACK == node->right->right->color);
		CU_ASSERT(NULL == node->right->right->right);
		CU_ASSERT( 6 == node->right->right->left->key);
		CU_ASSERT(RED == node->right->right->left->color);
		CU_ASSERT(NULL == node->right->right->left->left);
		CU_ASSERT(NULL == node->right->right->left->right);

		// 8. delete 3
		red_black_delete(&node);
		node = tree_root();
		tree_print_dot("delete08.dot", node);

		CU_ASSERT( 4 == node->key);
		CU_ASSERT(NULL == node->parent);
		CU_ASSERT(BLACK == node->color);
		CU_ASSERT( 1 == node->left->key);
		CU_ASSERT(BLACK == node->left->color);
		CU_ASSERT(NULL == node->left->left);
		CU_ASSERT( 6 == node->right->key);
		CU_ASSERT(RED == node->right->color);
		CU_ASSERT( 2 == node->left->right->key);
		CU_ASSERT(RED == node->left->right->color);
		CU_ASSERT(NULL == node->left->right->left);
		CU_ASSERT(NULL == node->left->right->right);
		CU_ASSERT( 5 == node->right->left->key);
		CU_ASSERT(BLACK == node->right->left->color);
		CU_ASSERT(NULL == node->right->left->left);
		CU_ASSERT(NULL == node->right->left->right);
		CU_ASSERT(14 == node->right->right->key);
		CU_ASSERT(BLACK == node->right->right->color);
		CU_ASSERT(NULL == node->right->right->right);
		CU_ASSERT(NULL == node->right->right->left);

		// 9. delete 4
		red_black_delete(&node);
		node = tree_root();
		tree_print_dot("delete09.dot", node);

		CU_ASSERT( 5 == node->key);
		CU_ASSERT(NULL == node->parent);
		CU_ASSERT(BLACK == node->color);
		CU_ASSERT( 1 == node->left->key);
		CU_ASSERT(BLACK == node->left->color);
		CU_ASSERT(NULL == node->left->left);
		CU_ASSERT( 6 == node->right->key);
		CU_ASSERT(BLACK == node->right->color);
		CU_ASSERT(NULL == node->right->left);
		CU_ASSERT( 2 == node->left->right->key);
		CU_ASSERT(RED == node->left->right->color);
		CU_ASSERT(NULL == node->left->right->left);
		CU_ASSERT(NULL == node->left->right->right);
		CU_ASSERT(14 == node->right->right->key);
		CU_ASSERT(RED == node->right->right->color);
		CU_ASSERT(NULL == node->right->right->right);
		CU_ASSERT(NULL == node->right->right->left);

		// 10. delete 5
		red_black_delete(&node);
		node = tree_root();
		tree_print_dot("delete10.dot", node);

		CU_ASSERT( 6 == node->key);
		CU_ASSERT(NULL == node->parent);
		CU_ASSERT(BLACK == node->color);
		CU_ASSERT( 1 == node->left->key);
		CU_ASSERT(BLACK == node->left->color);
		CU_ASSERT(NULL == node->left->left);
		CU_ASSERT(14 == node->right->key);
		CU_ASSERT(BLACK == node->right->color);
		CU_ASSERT(NULL == node->right->left);
		CU_ASSERT(NULL == node->right->right);
		CU_ASSERT( 2 == node->left->right->key);
		CU_ASSERT(RED == node->left->right->color);
		CU_ASSERT(NULL == node->left->right->left);
		CU_ASSERT(NULL == node->left->right->right);

		// 11. delete 6
		red_black_delete(&node);
		node = tree_root();
		tree_print_dot("delete11.dot", node);

		// NB: other red-black trees here result in a 1/2\14 setup (bug?)
		CU_ASSERT( 1 == node->key);
		CU_ASSERT(NULL == node->parent);
		CU_ASSERT(BLACK == node->color);
		CU_ASSERT(14 == node->right->key);
		CU_ASSERT(BLACK == node->right->color);
		CU_ASSERT(NULL == node->right->right);
		CU_ASSERT( 2 == node->right->left->key);
		CU_ASSERT(RED == node->right->left->color);
		CU_ASSERT(NULL == node->right->left->left);
		CU_ASSERT(NULL == node->right->left->right);

		// 12. delete 1
		red_black_delete(&node);
		node = tree_root();
		tree_print_dot("delete12.dot", node);

		CU_ASSERT(14 == node->key);
		CU_ASSERT(NULL == node->parent);
		CU_ASSERT(BLACK == node->color);
		CU_ASSERT(NULL == node->right);
		CU_ASSERT( 2 == node->left->key);
		CU_ASSERT(RED == node->left->color);
		CU_ASSERT(NULL == node->left->left);
		CU_ASSERT(NULL == node->left->right);

		// 13. delete 14
		red_black_delete(&node);
		node = tree_root();
		tree_print_dot("delete13.dot", node);

		CU_ASSERT(2 == node->key);
		CU_ASSERT(NULL == node->parent);
		CU_ASSERT(RED == node->color); // EXCEPTION: only a single node is RED
		CU_ASSERT(NULL == node->left);
		CU_ASSERT(NULL == node->right);

		// delete
		for (node = tree_root(); node != NULL; node = tree_root()) {
			red_black_delete(&node);
		}
	}

	// delete
	for (node = tree_root(); node != NULL; node = tree_root()) {
		red_black_delete(&node);
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
	red_black_insert(7, &values[7]);
	red_black_insert(11, &values[11]);
	red_black_insert(9, &values[9]);
	red_black_insert(13, &values[13]);
	node = tree_root();
	tree_print_dot("transplant00.dot", node);
	CU_ASSERT(9 == node->key);
	CU_ASSERT(11 == node->right->key);
       	{
		node = tree_root();
		node_p deletee = node->right;
		red_black_transplant(node->right, node->right->right);

		// delete
		free(deletee);
	}

	node = tree_root();
	tree_print_dot("transplant01.dot", node);
	CU_ASSERT(9 == node->key);
	CU_ASSERT(13 == node->right->key);

	// delete
	for (node = tree_root(); node != NULL; node = tree_root()) {
		red_black_delete(&node);
	}

        // left
	red_black_insert(3, &values[3]);
	red_black_insert(2, &values[2]);
	red_black_insert(7, &values[7]);
	red_black_insert(1, &values[1]);
	node = tree_root();
	tree_print_dot("transplant10.dot", node);
	CU_ASSERT(3 == node->key);
	CU_ASSERT(2 == node->left->key);
       	{
		node = tree_root();
		node_p deletee = node->left;
		red_black_transplant(node->left, node->left->left);

		// delete
		free(deletee);
	}

	node = tree_root();
	tree_print_dot("transplant11.dot", node);
	CU_ASSERT(3 == node->key);
	CU_ASSERT(1 == node->left->key);

	// delete
	for (node = tree_root(); node != NULL; node = tree_root()) {
		red_black_delete(&node);
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

	red_black_insert(7, &values[7]);

	red_black_insert(3, &values[3]);
	red_black_insert(11, &values[11]);

	red_black_insert(1, &values[1]);
	red_black_insert(5, &values[5]);
	red_black_insert(9, &values[9]);
	red_black_insert(13, &values[13]);

	red_black_insert(0, &values[0]);
	red_black_insert(2, &values[2]);
	red_black_insert(4, &values[4]);
	red_black_insert(6, &values[6]);
	red_black_insert(8, &values[8]);
	red_black_insert(10, &values[10]);
	red_black_insert(12, &values[12]);
	red_black_insert(14, &values[14]);

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
		red_black_delete(&node);
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

	red_black_insert(7, &values[7]);

	red_black_insert(3, &values[3]);
	red_black_insert(11, &values[11]);

	red_black_insert(1, &values[1]);
	red_black_insert(5, &values[5]);
	red_black_insert(9, &values[9]);
	red_black_insert(13, &values[13]);

	red_black_insert(0, &values[0]);
	red_black_insert(2, &values[2]);
	red_black_insert(4, &values[4]);
	red_black_insert(6, &values[6]);
	red_black_insert(8, &values[8]);
	red_black_insert(10, &values[10]);
	red_black_insert(12, &values[12]);
	red_black_insert(14, &values[14]);

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
		red_black_delete(&node);
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

	red_black_insert(7, &values[7]);

	red_black_insert(3, &values[3]);
	red_black_insert(11, &values[11]);

	red_black_insert(1, &values[1]);
	red_black_insert(5, &values[5]);
	red_black_insert(9, &values[9]);
	red_black_insert(13, &values[13]);

	red_black_insert(0, &values[0]);
	red_black_insert(2, &values[2]);
	red_black_insert(4, &values[4]);
	red_black_insert(6, &values[6]);
	red_black_insert(8, &values[8]);
	red_black_insert(10, &values[10]);
	red_black_insert(12, &values[12]);
	red_black_insert(14, &values[14]);

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
		red_black_delete(&node);
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

	red_black_insert(7, &values[7]);

	red_black_insert(3, &values[3]);
	red_black_insert(11, &values[11]);

	red_black_insert(1, &values[1]);
	red_black_insert(5, &values[5]);
	red_black_insert(9, &values[9]);
	red_black_insert(13, &values[13]);

	red_black_insert(0, &values[0]);
	red_black_insert(2, &values[2]);
	red_black_insert(4, &values[4]);
	red_black_insert(6, &values[6]);
	red_black_insert(8, &values[8]);
	red_black_insert(10, &values[10]);
	red_black_insert(12, &values[12]);
	red_black_insert(14, &values[14]);

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
		red_black_delete(&node);
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

	red_black_insert(7, &values[7]);

	red_black_insert(3, &values[3]);
	red_black_insert(11, &values[11]);

	red_black_insert(1, &values[1]);
	red_black_insert(5, &values[5]);
	red_black_insert(9, &values[9]);
	red_black_insert(13, &values[13]);

	red_black_insert(0, &values[0]);
	red_black_insert(2, &values[2]);
	red_black_insert(4, &values[4]);
	red_black_insert(6, &values[6]);
	red_black_insert(8, &values[8]);
	red_black_insert(10, &values[10]);
	red_black_insert(12, &values[12]);
	red_black_insert(14, &values[14]);

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
		red_black_delete(&node);
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

	red_black_insert(7, &values[7]);

	red_black_insert(3, &values[3]);
	red_black_insert(11, &values[11]);

	red_black_insert(1, &values[1]);
	red_black_insert(5, &values[5]);
	red_black_insert(9, &values[9]);
	red_black_insert(13, &values[13]);

	red_black_insert(0, &values[0]);
	red_black_insert(2, &values[2]);
	red_black_insert(4, &values[4]);
	red_black_insert(6, &values[6]);
	red_black_insert(8, &values[8]);
	red_black_insert(10, &values[10]);
	red_black_insert(12, &values[12]);
	red_black_insert(14, &values[14]);

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
		red_black_delete(&node);
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
	pSuite = CU_add_suite("red black tree", init_suite_redblacktree,
			      clean_suite_redblacktree);
	if (NULL == pSuite) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	/* utilities */
	TEST_start(pSuite, "tree root", test_tree_root)
 		TEST_append(pSuite, "tree get data", test_tree_get_data)
 		TEST_append(pSuite, "tree print", test_tree_print)
 		TEST_append(pSuite, "tree left rotate", test_tree_left_rotate)
 		TEST_append(pSuite, "tree right rotate", test_tree_right_rotate)
		TEST_append(pSuite, "tree insert fixup", test_red_black_insert_fixup)
		TEST_append(pSuite, "tree delete fixup", test_red_black_delete_fixup)
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

	CU_cleanup_registry();
	return CU_get_error();
}
