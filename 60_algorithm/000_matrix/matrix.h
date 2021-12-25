#ifndef MATRIX_H
#define MATRIX_H

# ifndef DEBUG
#  define DEBUG 1
# endif /* DEBUG */

#define MATRIX_NAME_SIZE 16
typedef struct matrix_s
{
	char name[MATRIX_NAME_SIZE];
	int **m;
	int nrows;
	int ncols;
} matrix_t;
typedef matrix_t* matrix_p;

void matrix_debug(const char* format, ...);
void matrix_failure(const char* format, ...);

matrix_p matrix_create(const char *name, int nrows, int ncols);
void matrix_init_all(matrix_p mat, int value);
void matrix_init_row(matrix_p mat, int rowidx, int* vals, int vals_size);
void matrix_destroy(matrix_p mat);
void matrix_print(matrix_p mat);


#endif /* MATRIX_H */
