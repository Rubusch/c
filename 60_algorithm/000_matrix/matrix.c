#include "matrix.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>


// formatted print
void matrix_debug(const char* format, ...)
{
#ifdef DEBUG
	va_list arglist;
	va_start(arglist, format);
	vprintf(format, arglist);
	va_end(arglist);
#endif
}

void matrix_failure(const char* format, ...)
{
	perror("failed!");
	va_list arglist;
	va_start(arglist, format);
	vprintf(format, arglist);
	va_end(arglist);
	exit(EXIT_FAILURE);
}


// matrix
matrix_p matrix_create(const char *name, int nrows, int ncols)
{
	matrix_p mat = malloc(sizeof(*mat));
	if (!mat) {
		matrix_failure("allocation failed");
	}

	mat->m = calloc(nrows, sizeof(*mat->m));

	for (int idx = 0; idx < nrows; idx++) {
		mat->m[idx] = calloc(ncols, sizeof(**mat->m));
		for (int jdx = 0; jdx < ncols; jdx++) {
			mat->m[idx][jdx] = (TYPE) 0;
		}
	}
	if (strlen(name) >= MATRIX_NAME_SIZE) {
		matrix_failure("matrix name overrun");
	}
	memset(mat->name, '\0', (MATRIX_NAME_SIZE-1) * sizeof(*mat->name));
	strncpy(mat->name, name, strlen(name));
	mat->name[MATRIX_NAME_SIZE-1] = '\0';
	mat->ncols = ncols;
	mat->nrows = nrows;
	return mat;
}

void matrix_destroy(matrix_p *mat)
{
	if (!mat) return;

	matrix_p deletee = *mat;
	if (!deletee) return;

	for (int idx = 0; idx < deletee->nrows; idx++) {
		free(deletee->m[idx]);
	}
	free(deletee->m);
	free(*mat);
	*mat = NULL;
}

void matrix_init_all(matrix_p mat, TYPE value)
{
	if (!mat) return;
	for (int idx = 0; idx < mat->nrows; idx++) {
		for (int jdx = 0; jdx < mat->ncols; jdx++) {
			mat->m[idx][jdx] = value;
		}
	}
}

void matrix_init_row(matrix_p mat, int rowidx, TYPE* vals, int vals_size)
{
	if (!mat) return;
	if (rowidx >= mat->nrows) return;
	if (vals_size != mat->ncols) return;

	for (int idx = 0; idx < mat->ncols; idx++) {
		mat->m[rowidx][idx] = vals[idx];
	}
}


void int_matrix_print(matrix_p mat)
{
#ifdef DEBUG
	if (!mat) {
		return;
	}
	matrix_debug("\n%s =\n", mat->name);
	for (int idx = 0; idx < mat->nrows; idx++) {
		for (int jdx = 0; jdx < mat->ncols; jdx++) {
			matrix_debug("%d ", (int) mat->m[idx][jdx]);
		}
		matrix_debug("\n");
	}
#endif /* DEBUG */
}

void char_matrix_print(matrix_p mat)
{
#ifdef DEBUG
	if (!mat) {
		return;
	}
	matrix_debug("\n%s =\n", mat->name);
	for (int idx = 0; idx < mat->nrows; idx++) {
		for (int jdx = 0; jdx < mat->ncols; jdx++) {
			matrix_debug("%c ", (char) mat->m[idx][jdx]);
		}
		matrix_debug("\n");
	}
#endif /* DEBUG */
}

void double_matrix_print(matrix_p mat)
{
#ifdef DEBUG
	if (!mat) {
		return;
	}
	matrix_debug("\n%s =\n", mat->name);
	for (int idx = 0; idx < mat->nrows; idx++) {
		for (int jdx = 0; jdx < mat->ncols; jdx++) {
			matrix_debug("%f ", (double) mat->m[idx][jdx]);
		}
		matrix_debug("\n");
	}
#endif /* DEBUG */
}

