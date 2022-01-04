#ifndef TEST_H
#define TEST_H


# ifndef TRUE
#  define TRUE 1
# endif

# ifndef FALSE
#  define FALSE 0
# endif

#include <stdio.h>
#include <stdbool.h>

#include "CUnit/Basic.h"
#include "CUnit/Console.h"
#include "CUnit/Automated.h"
#include "CUnit/CUCurses.h"

bool rel_eq_dbl(double a, double b);

/*
  TEST helper macros

  suite: pSuite
  name:  "a descriptor"
  func:  test_swap for function "void test_swap(void)"
 */
#define TEST_start(suite, name, func) \
	if ( \
	   (NULL == CU_add_test(suite, name, func)) \

#define TEST_append(suite, name, func) \
	|| (NULL == CU_add_test(suite, name, func))

#define TEST_end() \
	) { \
		CU_cleanup_registry(); \
		return CU_get_error(); \
	}

int init_suite_dynamic_programming(void) { return 0; }
int clean_suite_dynamic_programming(void) { return 0; }


#endif
