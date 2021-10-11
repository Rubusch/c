// tree_control.c
/*
  Adapter functions for the frontend to handle the a tree in the background
//*/

#include "tree_control.h"

/*
  quit()

  quits the prog, refreshes the number of elements in the tree
//*/
int quit(unsigned int *elements)
{
	if (NULL == backend_get_root()) {
		fprintf(stderr, "ERROR: quit() - backend_get_root() is NULL\n");
		return -1;
	}
	if (NULL == elements) {
		fprintf(stderr, "ERROR: quit() - number of elements is NULL\n");
		return -1;
	}

	if (0 > delete_tree(elements)) {
		return -1;
	}

	return 0;
}

/*
  init_root()

  inits the root node with data
  refreshes the number of elements
//*/
int init_root(unsigned int *elements)
{
	if (NULL != backend_get_root()) {
		fprintf(stderr,
			"ERROR: init_root() - backend_get_root() is NOT NULL\n");
		return -1;
	}
	if (NULL == elements) {
		fprintf(stderr,
			"ERROR: init_root() - number of elements is NULL\n");
		return -1;
	}

	if (0 > backend_create_root()) {
		return -1;
	}

	*elements = 1;
	return 0;
}

/*
  populate_tree()

  populates the tree starting from first with number of new elements
  refreshes the number of elements
//*/
int populate_tree(unsigned int *elements, const unsigned int number)
{
	if (NULL == backend_get_root()) {
		fprintf(stderr,
			"ERROR: populate_tree() - backend_get_root() is NULL\n");
		return -1;
	}
	if (NULL == elements) {
		fprintf(stderr,
			"ERROR: populate_tree() - number of elements is NULL\n");
		return -1;
	}

	register unsigned int cnt = 0;
	register unsigned int idx_break = 0;
	for (cnt = 0; (number > cnt) && (MAX_NODES > *elements); ++idx_break) {
		if (0 == backend_insert_node(backend_set_data_automatically(
				 backend_create_node()))) {
			++(*elements);
			++cnt;
		}

		if (MAX_NODES <= idx_break)
			break;
	}

	return 0;
}

/*
  add_element()

  adds a new element into the tree
  refreshes the number of elements
//*/
int add_element(unsigned int *number)
{
	if (NULL == backend_get_root()) {
		fprintf(stderr,
			"ERROR: add_element() - backend_get_root() is NULL\n");
		return -1;
	}
	if (NULL == number) {
		fprintf(stderr,
			"ERROR: add_element() - number of elements is NULL\n");
		return -1;
	}
	if (MAX_NODES <= *number) {
		fprintf(stderr,
			"ERROR: add_element() - maximum number of nodes, not inserting\n");
		return -1;
	}

	if (0 > backend_insert_node(backend_set_data(backend_create_node()))) {
		return -1;
	}

	++(*number);

	return 0;
}

/*
  find_element()

  finds an element by the given data content
  set's the pointer found to it
//*/
int find_element()
{
	if (NULL == backend_get_root()) {
		fprintf(stderr,
			"ERROR: find_element() - backend_get_root() is NULL\n");
		return -1;
	}

	backend_set_node_ptr(backend_create_node());
	if (NULL !=
	    backend_find_node(backend_set_data(backend_get_node_ptr()))) {
		backend_free_node_ptr();
		return 0;
	} else {
		backend_free_node_ptr();
		return -1;
	}
}

/*
  delete_leaf()

  finds the node and predecessor
  deletes an element of the tree
  refreshes the number of elements
//*/
int delete_leaf(unsigned int *number)
{
	if (NULL == backend_get_root()) {
		fprintf(stderr,
			"ERROR: delete_leaf() - backend_get_root() is NULL\n");
		return -1;
	}
	if (NULL == number) {
		fprintf(stderr,
			"ERROR: delete_leaf() - number of elements is NULL\n");
		return -1;
	}

	backend_set_node_ptr(backend_create_node());
	if (0 > backend_find_and_delete_node(
			backend_set_data(backend_get_node_ptr()))) {
		backend_free_node_ptr();
		return -1;
	}
	backend_free_node_ptr();
	--(*number);

	return 0;
}

/*
  delete_tree()

  deletes the entier tree
  refreshes the elements counter
//*/
int delete_tree(unsigned int *number)
{
	if (NULL == backend_get_root()) {
		fprintf(stderr,
			"ERROR: delete_tree() - backend_get_root() is NULL\n");
		return -1;
	}

	if (NULL == number) {
		fprintf(stderr,
			"ERROR: delete_tree() - number of elements is NULL\n");
		return -1;
	}

	if (0 > backend_delete_all()) {
		return -1;
	}

	*number = 0;
	return 0;
}

/*
  init_path()

  returns wether the current position in the path mask is a '1', '0' or 'x'
  it calculates the interval of rows containining a 1 or 0 and one x
  first it looks up if the current line and column is at a position of 'x'
  then it calculates wether the current line is in an interval of '1' or '0'
//*/
int init_path(char *path, const unsigned int idx_row,
	      const unsigned int path_size)
{
	register unsigned int interval;
	register unsigned int result;

	register unsigned int idx = 0;
	for (idx = 0; idx < path_size - 1; ++idx) {
		// no path to set
		if (path_size == 0)
			return 0;

		// check for 'x' before -> 'x' next
		if ((0 < idx) && (path[idx - 1] == 'x')) {
			path[idx] = 'x';
		}

		// get interval size:  2-1=1, 4-1=3, 8-1=7,...
		interval = (1 << ((path_size - 1) - idx)) - 1;
		result = idx_row % (2 * (interval + 1));

		if (result == interval) {
			// set 'x'
			path[idx] = 'x';

		} else if (result < interval) {
			// set '1'
			path[idx] = '1';

		} else {
			// set '0'
			path[idx] = '0';
		}
	}

	return 0;
}

/*
  get_data_by_path()

  follows the given path and fetches the element->data
  transforms element->data into string
  and writes it into data
  returns 0 on success, else -1 (element was NULL!)
  or -2
//*/
int get_data_by_path(char *data, const unsigned int data_len, char *path,
		     const unsigned int path_size, const unsigned int idx_col)
{
	if (NULL == backend_get_root()) {
		perror("ERROR: get_data_by_path() failed, backend_get_root() was NULL");
		return -1;
	}

	// in case of odd value - non data column
	if (idx_col % 2 != 0)
		return 0;
	backend_set_node_ptr(backend_get_root());

	// get element
	unsigned int idx_path = 0;
	for (idx_path = 0;
	     (idx_path <= idx_col / 2) && (NULL != backend_get_node_ptr());
	     ++idx_path) {
		if (path[idx_path] == '1') {
			backend_set_node_ptr(
				backend_get_right_node(backend_get_node_ptr()));
		} else if (path[idx_path] == '0') {
			backend_set_node_ptr(
				backend_get_left_node(backend_get_node_ptr()));
		} else {
			break;
		}
	}

	// check if NULL, if so set rest of path to 'x'
	// and set data to an empty string of data_len (spaceholder)
	if (NULL == backend_get_node_ptr()) {
		for (; idx_path < path_size; ++idx_path)
			path[idx_path] = 'x';
		//    strncpy(data, " N/A ", data_len);
		strncpy(data, " N/A ", data_len);
		data[data_len] = '\0';

	} else {
		// convert to string
		data_to_string(data, (data_len + 1),
			       backend_get_data(backend_get_node_ptr()));
	}

	return 0;
}

/*
  get_paint_mask_per_row()

  returns wether in this line a data entry is necessary or not
//*/
int get_paint_mask_per_row(unsigned int *paint, const unsigned int paint_size,
			   const unsigned int idx_row_max,
			   const unsigned int idx_row)
{
	register unsigned int tree_level = 0;
	register unsigned int shift_data = 0;
	register unsigned int interval = 0;
	register unsigned int shift_connect = 0;
	register unsigned int idx_row_shifted = 0;

	register unsigned int idx_col = 0;
	for (idx_col = 0; idx_col < paint_size; ++idx_col) {
		if (0 == idx_col % 2) {
			// elements
			tree_level = idx_col / 2;
			shift_data = (idx_row_max + 2) / (1 << tree_level);
			if ((idx_row >= (shift_data / 2 - 1)) &&
			    (0 ==
			     (idx_row + 1 - (shift_data / 2)) % shift_data)) {
				paint[idx_col] = 1;
			} else {
				paint[idx_col] = 0;
			}

		} else {
			// connectors
			interval = 1 + idx_row_max / (2 << ((idx_col - 1) / 2));
			shift_connect = interval / 2;
			if (idx_col == paint_size - 2) {
				if (paint[idx_col - 1] == 1) {
					paint[idx_col] = 1;
				} else {
					paint[idx_col] = 0;
				}
			} else if ((idx_row >= shift_connect) &&
				   (idx_row <= idx_row_max - shift_connect)) {
				idx_row_shifted = idx_row - shift_connect;
				if ((idx_row + 1 != interval) &&
				    ((idx_row_shifted % (2 * interval)) <
				     (interval - 1)) &&
				    (paint[idx_col - 1] != 1)) {
					paint[idx_col] = 1;
				} else {
					paint[idx_col] = 0;
				}
			} else {
				paint[idx_col] = 0;
			}
		}
	}
	return 0;
}

/*
  write_tree()

  creates lines to write,
  works line by line and doesn't store
  an array based template!

    path
    1 = right
    0 = left
    x = not set

    paint
    even idexes:
    0 - no element
    1 - element

    odd indexes:
    0 - not set
    1 - set

  returns -1 in case of errors
//*/
// TODO: improve!
int write_tree()
{
	if (NULL == backend_get_root())
		return -1;

	// idx_col
	register unsigned int idx_col = 0;

	// idx_col_max
	const unsigned int idx_col_max = 2 * backend_get_max_level();

	if (0 > idx_col_max) {
		perror("ERROR: write_tree() - backend_get_max_level() failed!");
		return -1;
	}

	// idx_row
	register unsigned int idx_row = 0;

	// idx_row_max
	unsigned int idx_row_max = 2 * (1 << (idx_col_max / 2)) - 2;

	// paint currently
	unsigned int paint[idx_col_max + 1];
	{
		register unsigned int cnt = 0;
		for (cnt = 0; cnt < idx_col_max + 1; ++cnt) {
			paint[cnt] = 0;
		}
	}

	// path - TODO: use bitmask
	char path[(idx_col_max / 2) + 1];
	memset(path, '1', idx_col_max / 2);
	path[idx_col_max / 2] = '\0';

	// data - length of data: ' ' + data + ' '
	const unsigned int data_len = DATA_DIGITS + 2;

	// data field
	char data[data_len + 1];

	// line_size - current line = data length + spaces + '\n' + '\0'
	const unsigned int line_size =
		(idx_col_max / 2 + 1) * data_len + (idx_col_max / 2 + 1) + 2;

	// curr_line
	char curr_line[line_size];
	memset(curr_line, '\0', line_size);

	// open file
	FILE *fp = NULL;
	if (0 > get_write_file_pointer(&fp, "treeoutput.txt")) {
		perror("ERROR: write_tree() - file pointer failed");
		return -1;
	}

	for (idx_row = 0; idx_row <= idx_row_max; ++idx_row) {
		// reset row_write index
		memset(curr_line, '\0', line_size);

		// set up mask
		if (0 > get_paint_mask_per_row(paint, idx_col_max + 1,
					       idx_row_max, idx_row)) {
			perror("ERROR: write_tree() - could not set up paint mask!");
			return -1;
		}

		// init path
		if (0 > init_path(path, idx_row, (idx_col_max / 2) + 1)) {
			perror("ERROR: write_tree() - colomn index out of memory!");
			return -1;
		}

		for (idx_col = 0; (idx_col <= idx_col_max); ++idx_col) {
			// current row has an element->data line?
			memset(data, '\0', data_len + 1);

			if (idx_col % 2 == 0) {
				if (0 < paint[idx_col]) {
					// row to write data
					get_data_by_path(data, data_len, path,
							 idx_col_max / 2 + 1,
							 idx_col);
				} else {
					// no data to write
					memset(data, ' ', data_len);
				}

			} else {
				if (0 < paint[idx_col]) {
					// connector
					if (idx_col == idx_col_max - 1) {
						data[0] = '<';
					} else if ((idx_col_max > 3) &&
						   (idx_col ==
						    idx_col_max - 3)) {
						if ((idx_row > 1) &&
						    (((idx_row - 2) % 8) ==
						     0)) {
							data[0] = '/';
						} else {
							data[0] = '\\';
						}
					} else {
						data[0] = '|';
					}
				} else {
					// no connector
					strncpy(data, " ", 2);
				}
			}
			strncat(curr_line, data, data_len);
		}

		// terminate line with '\n' and '\0'
		strncat(curr_line, "\n", 1);

		// write to file
		if (0 > write_linewise(fp, curr_line, line_size)) {
			perror("ERROR: write_tree() - writing linewise failed");
			return -1;
		}
	}

	if (0 > close_stream(&fp)) {
		perror("ERROR: write_tree() - closing the filepointer failed");
		return -1;
	}

	return 0;
}

/*
  deletes any element in the tree (edge and leaf)
//*/
int delete_element()
{
	backend_set_node_ptr(backend_set_data(backend_create_node()));
	backend_find_and_delete_node(backend_get_node_ptr());
	backend_free_node_ptr();
	return 0;
}

/*
  does a backup of an element into the root_clone
//*/
int backup_element()
{
	backend_set_node_ptr(backend_set_data(backend_create_node()));
	if (0 > backend_backup_node(backend_get_node_ptr())) {
		backend_free_node_ptr();
		return -1;
	}
	backend_free_node_ptr();
	return 0;
}

/*
  does a backup of the tree to the root_clone
//*/
int backup_tree()
{
	backend_set_node_ptr(backend_create_node());
	if (0 > backend_backup_tree(backend_get_node_ptr())) {
		backend_free_node_ptr();
		return -1;
	}
	backend_free_node_ptr();
	return 0;
}

/*
  iterates in pre order and prints out the content of each element
//*/
int traverse_pre_order(unsigned int *number_of_elements)
{
	if (NULL == number_of_elements)
		return -1;

	backend_set_node_ptr(backend_get_leftest());
	unsigned int cnt = 0;
	for (cnt = 0; cnt < *number_of_elements; ++cnt) {
		backend_set_node_ptr(
			backend_traverse_pre_order(backend_get_node_ptr()));
		if (0 > backend_print_data(backend_get_node_ptr())) {
			return -1;
		}
	}

	return 0;
}

/*
  iterates in post order and prints out the content of each element
//*/
int traverse_post_order(unsigned int *number_of_elements)
{
	if (NULL == number_of_elements)
		return -1;

	backend_set_node_ptr(backend_get_leftest());
	unsigned int cnt = 0;
	for (cnt = 0; cnt < *number_of_elements; ++cnt) {
		backend_set_node_ptr(
			backend_traverse_post_order(backend_get_node_ptr()));
		if (0 > backend_print_data(backend_get_node_ptr())) {
			return -1;
		}
	}

	return 0;
}

/*
  iterates in in order and prints out the content of each element
//*/
int traverse_in_order(unsigned int *number_of_elements)
{
	if (NULL == number_of_elements)
		return -1;

	backend_set_node_ptr(backend_get_leftest());
	unsigned int cnt = 0;
	for (cnt = 0; cnt < *number_of_elements; ++cnt) {
		backend_set_node_ptr(
			backend_traverse_in_order(backend_get_node_ptr()));
		if (0 > backend_print_data(backend_get_node_ptr())) {
			return -1;
		}
	}

	return 0;
}

/*
  perform a left rotation
//*/
int rotate_left()
{
	backend_set_node_ptr(backend_create_node());
	if (0 >
	    backend_left_rotation(backend_set_data(backend_get_node_ptr()))) {
		backend_free_node_ptr();
		return -1;
	}
	backend_free_node_ptr();
	return 0;
}

/*
  perform a right rotation
//*/
int rotate_right()
{
	backend_set_node_ptr(backend_create_node());
	if (0 >
	    backend_right_rotation(backend_set_data(backend_get_node_ptr()))) {
		backend_free_node_ptr();
		return -1;
	}
	backend_free_node_ptr();
	return 0;
}
