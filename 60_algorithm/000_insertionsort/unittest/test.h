#ifndef TEST_H
#define TEST_H


# ifndef TRUE
#  define TRUE 1
# endif

# ifndef FALSE
#  define FALSE 0
# endif

#include "stdlib.h"
#include "stdio.h"
#include "time.h"

#include "CUnit/Basic.h"
#include "CUnit/Console.h"
#include "CUnit/Automated.h"
#include "CUnit/CUCurses.h"

#include "insertionsort.h"


/*
  TEST helper macros

  suite: pSuite
  name:  "a string"
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


int init_suite_success(void) { return 0; }
int clean_suite_success(void) { return 0; }


#endif
