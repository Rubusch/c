/*
  cunit - demo test

  REFERENCES:
  http://cunit.sourceforge.net/screenshots.html
*/

#ifndef TEST_H_
#define TEST_H_

# ifndef TRUE
#  define TRUE 1
# endif

# ifndef FALSE
#  define FALSE 0
# endif

#endif

#include "CUnit/Basic.h"
#include "CUnit/Console.h"
#include "CUnit/Automated.h"

#include "utility.h"

int init_suite_success(void) { return 0; }
int clean_suite_success(void) { return 0; }


void test__my_add(void)
{
	int x = 1;
	int y = 3;
	int expected = 4;
	CU_ASSERT(expected == my_add(x,y));
}

void test__my_multiply(void)
{
	int x = 1;
	int y = 3;
	int expected = 3;
	CU_ASSERT(expected == my_multiply(x,y));
}


int main()
{
   CU_pSuite pSuite = NULL;

   /* initialize the CUnit test registry */
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

   /* add a suite to the registry */
   pSuite = CU_add_suite("Suite_success", init_suite_success, clean_suite_success);
   if (NULL == pSuite) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the suite */
   if ((NULL == CU_add_test(pSuite, "successful_test_1", test__my_add)) ||
       (NULL == CU_add_test(pSuite, "successful_test_2", test__my_multiply)))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* Run all tests using the basic interface */
   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();
   printf("\n");
   CU_basic_show_failures(CU_get_failure_list());
   printf("\n\n");

   /* Clean up registry and return */
   CU_cleanup_registry();
   return CU_get_error();
}
