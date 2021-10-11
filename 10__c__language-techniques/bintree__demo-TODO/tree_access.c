// tree_access.c
/*
 * @author: Lothar Rubusch
 * @email: L.Rubusch@gmx.ch
 * @license: GPLv3
 *
 * Adapter functions for the frontend to handle the a tree in the background
//*/

#include "tree_access.h"

/*
  quit()

  quits the prog, refreshes the number of elements in the tree
//*/
int quit(leaf **first, unsigned int *elements)
{
	if (NULL == first) {
		fprintf(stderr, "ERROR: quit() - first is NULL\n");
		return -1;
	}
	if (NULL == elements) {
		fprintf(stderr, "ERROR: quit() - number of elements is NULL\n");
		return -1;
	}

	if (0 > delete_tree(first, elements)) {
		return -1;
	}

	return 0;
}

/*
  init_root()

  inits the root node with data
  refreshes the number of elements
//*/
int init_root(leaf **first, unsigned int *elements, unsigned int data)
{
	if (NULL != *first) {
		fprintf(stderr, "ERROR: init_root() - first is NOT NULL\n");
		return -1;
	}
	if (NULL == elements) {
		fprintf(stderr,
			"ERROR: init_root() - number of elements is NULL\n");
		return -1;
	}

	if (0 > init_node(first, data)) {
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
int populate_tree(leaf *first, unsigned int *elements,
		  const unsigned int number)
{
	if (NULL == first) {
		fprintf(stderr, "ERROR: populate_tree() - first is NULL\n");
		return -1;
	}
	if (NULL == elements) {
		fprintf(stderr,
			"ERROR: populate_tree() - number of elements is NULL\n");
		return -1;
	}

	unsigned int data = 0;
	leaf *lf = NULL;

	register unsigned int idx = 0;
	register unsigned int cnt_new_elements = 0;
	for (idx = 0; (number > cnt_new_elements) && (MAX_NODES > number) &&
		      (MAX_NODES > idx);
	     ++idx) {
		lf = NULL;
		data = random() % MAX_DATA_NUMBER;

		if (0 > init_node(&lf, data)) {
			fprintf(stderr,
				"ERROR: populate_tree() - failed due to init of node.\n");
			return -1;
		}

		if (0 > insert(first, lf)) {
			fprintf(stderr,
				"populate_tree() - %d containted in the tree? Not inserted!\n",
				lf->data);
			if (NULL != lf)
				free(lf);
		} else {
			++cnt_new_elements;
			++(*elements);
		}
	}

	return 0;
}

/*
  add_element()

  adds a new element into the tree
  refreshes the number of elements
//*/
int add_element(leaf *first, unsigned int *number, const unsigned int data)
{
	if (NULL == first) {
		fprintf(stderr, "ERROR: add_element() - first is NULL\n");
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

	leaf *element = NULL;
	if (0 > init_node(&element, data)) {
		return -1;
	}
	if (0 > insert(first, element)) {
		if (NULL != element)
			free(element);
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
int find_element(leaf *first, leaf *found, const unsigned int data)
{
	if (NULL == first) {
		fprintf(stderr, "ERROR: find_element() - first is NULL\n");
		return -1;
	}

	if (NULL != (found = find_node(first, data))) {
		return 0;
	} else {
		return -1;
	}
}

/*
  delete_element()

  finds the node and predecessor
  deletes an element of the tree
  refreshes the number of elements
//*/
int delete_element(leaf *first, unsigned int *number, const unsigned int data)
{
	if (NULL == first) {
		fprintf(stderr, "ERROR: delete_element() - first is NULL\n");
		return -1;
	}
	if (NULL == number) {
		fprintf(stderr,
			"ERROR: delete_elemente() - number of elements is NULL\n");
		return -1;
	}

	leaf *delnode = NULL;
	leaf *dummy = first;
	if (first->data == data) {
		fprintf(stderr,
			"ERROR: delete_element() - element was root node\n");
		return -1;
	} else if (NULL != (delnode = find(first->left, &dummy, data))) {
		// ok found, next step: delete
	} else if (NULL != (delnode = find(first->right, &dummy, data))) {
		// ok found, next step: delete
	} else {
		fprintf(stderr,
			"ERROR: delete_elemente() - element to delete not found\n");
		return -1;
	}

	if (0 > delete (&delnode, first)) { // first here is predecessor!
		return -1;
	}

	--(*number);

	return 0;
}

/*
  delete_tree()

  deletes the entier tree
  refreshes the elements counter
//*/
int delete_tree(leaf **first, unsigned int *number)
{
	if (NULL == *first) {
		fprintf(stderr, "ERROR: delete_tree() - first is NULL\n");
		return -1;
	}
	if (NULL == number) {
		fprintf(stderr,
			"ERROR: delete_tree() - number of elements is NULL\n");
		return -1;
	}

	if (0 > delete_nodes(first)) {
		return -1;
	}

	*number = 0;
	return 0;
}

/*
  get_max_level()

  runs through the tree and finds the max tree level, recursively
  returns the current level
//*/
int get_max_level(leaf *node, unsigned int level)
{
	//  puts("get_max_level()"); // XXX

	unsigned int tmp = 1 + level;
	unsigned int result_level = level;

	leaf *left = node;
	leaf *right = node;
	if (0 <= get_left_node(&left)) {
		tmp = get_max_level(left, level + 1);
		result_level = tmp;
	}

	if (0 <= get_right_node(&right)) {
		tmp = get_max_level(right, level + 1);
		result_level = MAX(tmp, result_level);
	}

	// fprintf(stderr, "\tget_max_level() - returns %d\n", level); // XXX
	if (result_level > MAX_LEVEL)
		return MAX_LEVEL;
	return result_level;
}

/*
  data_to_string()

  transform a data value to string "element",
  add leading and tailing ' '
  if the int doesn't fit the size append ' '

  the space of element needs to be allocated before!
//*/
int data_to_string(char *str, const unsigned int str_size,
		   const unsigned int data)
{
	//  puts("\tdata_to_string()");

	// leading
	strncat(str, " ", 2);

	// convert data
	register unsigned int num = 0;
	if (0 > (num = sprintf((++str), "%3d", data))) {
		perror("ERROR: data_to_string() failed somehow");
		return -1;
	} else if (num < DATA_DIGITS) {
		register unsigned int idx = 0;
		for (idx = num; idx < DATA_DIGITS; ++idx) {
			strncat(str, " ", 2);
		}
	}

	// tailing ' '
	strncat(str, " ", 2);

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
	//   puts("init_path()"); // XXX

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

	//   fprintf(stderr, "path: #%s#\n", path); // XXX

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
int get_data_by_path(leaf *first, char *data, const unsigned int data_len,
		     char *path, const unsigned int path_size,
		     const unsigned int idx_col)
{
	//  puts("get_data_by_path()"); // XXX

	if (first == NULL) {
		perror("ERROR: get_data_by_path() failed, first was NULL");
		return -1;
	}

	// in case of odd value - non data column
	if (idx_col % 2 != 0)
		return 0;
	leaf *element = first;

	// get element
	unsigned int idx_path = 0;
	for (idx_path = 0; (idx_path <= idx_col / 2) && (NULL != element);
	     ++idx_path) {
		if (path[idx_path] == '1') {
			if (0 > get_right_node(&element)) {
				element = NULL;
			}
		} else if (path[idx_path] == '0') {
			if (0 > get_left_node(&element)) {
				element = NULL;
			}
		} else {
			break;
		}
	}

	// check if NULL, if so set rest of path to 'x'
	// and set data to an empty string of data_len (spaceholder)
	if (element == NULL) {
		for (; idx_path < path_size; ++idx_path)
			path[idx_path] = 'x';
		strncpy(data, " N/A ", data_len);
		data[data_len] = '\0';

	} else {
		// convert to string
		data_to_string(data, (data_len + 1), element->data);
	}

	//  fprintf(stderr, "data: #%s#\n", data); // XXX

	return 0;
}

/*
  get_paint_mask_per_row()

  returns wether in this line a data entry is necessary or not
//*/
int get_paint_mask_per_row(char *paint, const unsigned int paint_size,
			   const unsigned int idx_row_max,
			   const unsigned int idx_row)
{
	register unsigned int tree_level = 0;
	register unsigned int shift_data = 0;
	register unsigned int interval = 0;
	register unsigned int shift_connect = 0;
	register unsigned int idx_row_shifted = 0;

	register unsigned int idx_col = 0;
	for (idx_col = 0; idx_col < paint_size - 1; ++idx_col) {
		if (idx_col % 2 == 0) {
			// elements
			tree_level = idx_col / 2;
			shift_data = (idx_row_max + 2) / (1 << tree_level);

			if ((idx_row >= (shift_data / 2 - 1)) &&
			    (0 ==
			     (idx_row + 1 - (shift_data / 2)) % shift_data)) {
				paint[idx_col] = '1';
			} else {
				paint[idx_col] = '0';
			}

		} else {
			// connectors
			interval = 1 + idx_row_max / (2 << ((idx_col - 1) / 2));
			shift_connect = interval / 2;

			if (idx_col == paint_size - 3) { // line to do the '<'s
				if (paint[idx_col - 1] == '1') {
					paint[idx_col] = '1';
				} else {
					paint[idx_col] = '0';
				}
			} else if ((idx_row >= shift_connect) &&
				   (idx_row <= idx_row_max - shift_connect)) {
				idx_row_shifted = idx_row - shift_connect;
				if ((idx_row + 1 != interval) &&
				    ((idx_row_shifted % (2 * interval)) <
				     (interval - 1)) &&
				    (paint[idx_col - 1] != '1')) {
					paint[idx_col] = '1';
				} else {
					paint[idx_col] = '0';
				}
			} else {
				paint[idx_col] = '0';
			}
		}
	}
	//  fprintf(stderr, "mask: #%s#\n", paint); // XXX
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
int write_tree(leaf *first)
{
	//  puts("write_tree()"); // XXX

	// idx_col
	register unsigned int idx_col = 0;

	// idx_col_max
	const unsigned int idx_col_max = 2 * get_max_level(first, 0);

	if (0 > idx_col_max) {
		perror("ERROR: write_tree() - get_max_level() failed!");
		return -1;
	}

	// idx_row
	register unsigned int idx_row = 0;

	// idx_row_max
	unsigned int idx_row_max = 2 * (1 << (idx_col_max / 2)) - 2;

	// paint currently - TODO: use bitmask
	char paint_curr[idx_col_max + 1];
	memset(paint_curr, '\0', idx_col_max + 1);

	// path - TODO: use bitmask
	char path[(idx_col_max / 2) + 1];
	memset(path, '1', idx_col_max / 2);
	path[idx_col_max / 2] = '\0';

	// data - length of data: ' ' + data + ' '
	const unsigned int data_len = DATA_DIGITS + 2;

	// data field
	char data[data_len + 1];

	// line_size - current line = data length + spaces + '\n' + '\0'
	const unsigned int line_size = (idx_col_max / 2 + 1) * data_len +
				       ((idx_col_max / 2 + 1) - 1) + 2;

	// curr_line
	char curr_line[line_size];
	memset(curr_line, '\0', line_size);

	// open file
	FILE *fp = NULL;
	if (0 > get_write_file_pointer(&fp, "treeoutput.txt")) {
		perror("ERROR: write_tree() - file pointer failed");
		return -1;
	}

	/*
    LOOP per row
  //*/
	for (idx_row = 0; idx_row <= idx_row_max; ++idx_row) {
		// reset row_write index
		memset(curr_line, '\0', line_size);

		// init path
		if (0 > init_path(path, idx_row, (idx_col_max / 2) + 1)) {
			perror("ERROR: write_tree() - colomn index out of memory!");
			return -1;
		}

		// set up mask
		if (0 > get_paint_mask_per_row(paint_curr, idx_col_max + 2,
					       idx_row_max, idx_row)) {
			perror("ERROR: write_tree() - could not set up paint mask!");
			return -1;
		}

		/*
      LOOP per col in line: write curr_line col by col
    //*/
		for (idx_col = 0; (idx_col <= idx_col_max); ++idx_col) {
			// current row has an element->data line?
			memset(data, '\0', data_len + 1);

			if (idx_col % 2 == 0) {
				if (paint_curr[idx_col] == '1') {
					// row to write data
					get_data_by_path(first, data, data_len,
							 path,
							 idx_col_max / 2 + 1,
							 idx_col);
					paint_curr[idx_col] = '1';
				} else {
					// no data to write
					memset(data, ' ', data_len);
					paint_curr[idx_col] = '0';
				}

			} else {
				if (paint_curr[idx_col] == '1') {
					// connector
					if (idx_col == idx_col_max - 1) {
						data[0] = '<';
						paint_curr[idx_col] = '1';
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
						paint_curr[idx_col] = '1';
					}
				} else {
					// no connector
					strncpy(data, " ", 2);
					paint_curr[idx_col] = '0';
				}
			}
			strncat(curr_line, data, data_len);

			/*
        finish column
      //*/
		}

		/*
      finish row
    //*/

		// terminate line with '\n' and '\0'
		strncat(curr_line, "\n", 1);

		// write to file
		if (0 > write_linewise(fp, curr_line, line_size)) {
			perror("ERROR: write_tree() - writing linewise failed");
			return -1;
		}
	}

	/*
    finish writing to file
  //*/

	if (0 > close_stream(&fp)) {
		perror("ERROR: write_tree() - closing the filepointer failed");
		return -1;
	}

	return 0;
}

/*
  File operations
//*/

/*
  get the file pointer
//*/
int get_write_file_pointer(FILE **fp, char filename[FILENAME_MAX])
{
	if (*fp != NULL)
		return -1;
	if (filename == NULL)
		return -1;

	if ((*fp = fopen(filename, "w")) == NULL) {
		fprintf(stderr,
			"fo::get_write_file_pointer(FILE**, char[]) - Failed!\n");
		return -1;
	}

	return 0;
}

/*
  write linewise
//*/
int write_linewise(FILE *fp, char *content,
		   const unsigned long int CONTENT_SIZE)
{
	if (fp == NULL)
		return -1;
	if (content == NULL)
		return -1;

	char bufLine[BUFSIZ];
	int idxLine = 0;
	int idxContent = 0;
	char *pData = &content[0];
	strcpy(bufLine, "");

	while ((idxLine < BUFSIZ) && (idxContent < CONTENT_SIZE) &&
	       ((bufLine[idxLine] = *(pData++)) != '\0')) {
		if (idxLine >= BUFSIZ) {
			fprintf(stderr,
				"fo::write_linewise(FILE*, char*, const unsigned long) - Failed!\n");
			return -1;
		}

		if (((idxLine == CONTENT_SIZE - 2) &&
		     (bufLine[idxLine] != '\n')) ||
		    (*(pData + 1) == '\0')) {
			bufLine[idxLine + 1] = '\0';
			fputs(bufLine, fp); // write line
			break;

		} else if (bufLine[idxLine] == '\n') {
			fputs(bufLine, fp); // write line
			idxLine = 0;
		} else {
			++idxLine;
		}
		++idxContent;
	}
	fputs("\n\0", fp); // tailing linefeed + linebreak
	return 0;
}

/*
  close the file pointer
//*/
int close_stream(FILE **fp)
{
	if (*fp == NULL)
		return -1;
	int iRes = fclose(*fp);
	*fp = NULL;

	return iRes;
}
