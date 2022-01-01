#ifndef MATRIX_H
#define MATRIX_H

# ifndef DEBUG
#  define DEBUG 1
# endif /* DEBUG */


/*
  part 1: the demo
 */
#define create_matrix(NAME, TYPE, MATRIX_NAME_SIZE, MATRIX_FAILURE_FUNC) \
									\
	typedef struct NAME##_matrix_s					\
	{								\
		char name[MATRIX_NAME_SIZE];				\
		TYPE **m;						\
		int nrows;						\
		int ncols;						\
	} NAME##_matrix_t;						\
	typedef NAME##_matrix_t* NAME##_matrix_p;			\
									\
	static inline NAME##_matrix_p NAME##_matrix_create(const char *name, \
							   int nrows, int ncols); \
									\
	static inline void NAME##_matrix_init_all(NAME##_matrix_p mat, TYPE value); \
									\
	static inline void NAME##_matrix_init_row(NAME##_matrix_p mat, int rowidx, \
						  TYPE* vals, int vals_size); \
									\
	static inline void NAME##_matrix_destroy(NAME##_matrix_p mat);	\
									\
	NAME##_matrix_p NAME##_matrix_create(const char *name, int nrows, int ncols) \
	{								\
		NAME##_matrix_p mat = malloc(sizeof(*mat));		\
		if (!mat) {						\
			MATRIX_FAILURE_FUNC("allocation failed");	\
		}							\
									\
		mat->m = calloc(nrows, sizeof(*mat->m));		\
		for (int idx = 0; idx < nrows; idx++) {			\
			mat->m[idx] = calloc(ncols, sizeof(**mat->m));	\
			for (int jdx = 0; jdx < ncols; jdx++) {		\
				mat->m[idx][jdx] = (TYPE) 0;		\
			}						\
		}							\
									\
		if (strlen(name) >= MATRIX_NAME_SIZE) {			\
			MATRIX_FAILURE_FUNC("matrix name overrun");	\
		}							\
		memset(mat->name, '\0', (MATRIX_NAME_SIZE-1) * sizeof(*mat->name)); \
		strncpy(mat->name, name, strlen(name));			\
		mat->name[MATRIX_NAME_SIZE-1] = '\0';			\
		mat->ncols = ncols;					\
		mat->nrows = nrows;					\
									\
		return mat;						\
	}								\
									\
	void NAME##_matrix_destroy(NAME##_matrix_p mat)			\
	{								\
		if (!mat) {						\
			return;						\
		}							\
									\
		for (int idx = 0; idx < mat->nrows; idx++) {		\
			free(mat->m[idx]);				\
		}							\
		free(mat->m);						\
		free(mat);						\
	}								\
									\
	void NAME##_matrix_init_all(NAME##_matrix_p mat, TYPE value)	\
	{								\
		if (!mat) return;					\
		for (int idx = 0; idx < mat->nrows; idx++) {		\
			for (int jdx = 0; jdx < mat->ncols; jdx++) {	\
				mat->m[idx][jdx] = value;		\
			}						\
		}							\
	}								\
									\
	void NAME##_matrix_init_row(NAME##_matrix_p mat, int rowidx,	\
				    TYPE* vals, int vals_size)		\
	{								\
		if (!mat) return;					\
		if (rowidx >= mat->nrows) return;			\
		if (vals_size != mat->ncols) return;			\
									\
		for (int idx = 0; idx < mat->ncols; idx++) {		\
			mat->m[rowidx][idx] = vals[idx];		\
		}							\
	}								\



/*
  part 2

  the following functions can be provided and in case reimplemented
  for other types - only matrix_failure (or similar) must be provided
 * /
void matrix_debug(const char* format, ...);
void matrix_failure(const char* format, ...);
void matrix_print(matrix_p mat);

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

void matrix_print(matrix_p mat)
{
	if (!mat) {
		return;
	}
	matrix_debug("\n%s =\n", mat->name);
	for (int idx = 0; idx < mat->nrows; idx++) {
		for (int jdx = 0; jdx < mat->ncols; jdx++) {
			matrix_debug("%d ", mat->m[idx][jdx]);
		}
		matrix_debug("\n");
	}
}
// */


/*
  part 3

 demo instantiation for the unittests (usage)
* /
create_matrix(my, int, 16, matrix_failure);
// */

#endif /* MATRIX_H */
