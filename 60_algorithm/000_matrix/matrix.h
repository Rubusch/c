#ifndef MATRIX_H
#define MATRIX_H

# ifndef DEBUG
#  define DEBUG 1
# endif /* DEBUG */

# ifndef TYPE
#  define TYPE int
//#  define TYPE char /* tested / verified */
//#  define TYPE double /* tested / verified */
# endif /* TYPE */

#define MATRIX_NAME_SIZE 16
typedef struct matrix_s
{
	char name[MATRIX_NAME_SIZE];
	TYPE **m;
	int nrows;
	int ncols;
} matrix_t;
typedef matrix_t* matrix_p;

// utils
void matrix_debug(const char* format, ...);
void matrix_failure(const char* format, ...);

// independent
matrix_p matrix_create(const char *name, int nrows, int ncols);
void matrix_destroy(matrix_p mat);
void matrix_init_all(matrix_p mat, TYPE value);
void matrix_init_row(matrix_p mat, int rowidx, TYPE* vals, int vals_size);

// type dependent
void int_matrix_print(matrix_p mat);
void char_matrix_print(matrix_p mat);
void double_matrix_print(matrix_p mat);


#endif /* MATRIX_H */
