#ifndef TEST_H
#define TEST_H


# ifndef TRUE
#  define TRUE 1
# endif

# ifndef FALSE
#  define FALSE 0
# endif


#define DEBUG 1


#include "stdio.h"
#include "time.h"

#include "CUnit/Basic.h"
#include "CUnit/Console.h"
#include "CUnit/Automated.h"
#include "CUnit/CUCurses.h"

#include "randomized-algorithm.h"


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

static candidate_t candidates[ARR_SIZE];

int init_suite_hire_assistant(void);
int clean_suite_hire_assistant(void) { return 0; }


#endif
