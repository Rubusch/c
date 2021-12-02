/*
  cunit - tests

 */
#include "test.h"
#include "stdlib.h"

int init_suite_hire_assistant(void)
{
	int upper = ARR_SIZE;
	int lower = 0;
	int idx;
	srand(time(0));
	for (idx=0; idx<ARR_SIZE; idx++) {
		candidates[idx].score = (rand() % (upper - lower + 1)) + lower;
		candidates[idx].idx = idx;
	}

	return 0;
}


void debug_print(const char* label, int idx)
{
#ifdef DEBUG
	fprintf(stderr, "\n%s: candidates[%d]\t== %d\n", label, idx, candidates[idx].score);
#endif
}

void test_hire_assistant(void)
{
	const int sufficient = 100;
	int res = -1;

	// testee
	res = hire_assistant(candidates, sufficient);

	debug_print("res (serial)", res);
	CU_ASSERT(res > -1);
}

void test_randomized_hire_assistant(void)
{
	const int sufficient = 100;
	int res = -1;

	// testee
	res = randomized_hire_assistant(candidates, sufficient);

	debug_print("res (randomized)", res);
	CU_ASSERT(res > -1); /* if we take 100 from 200.. */
}

void test_permute_by_sorting(void)
{
 	const int sufficient = 100;
	int res = -1;

	// testee
	res = permute_by_sorting(candidates, sufficient);

	debug_print("res (permute_by_sorting)", res);
	CU_ASSERT(res > -1);

	res = -1;

	// testee
	res = randomized_hire_assistant(candidates, sufficient);

	debug_print("res (permute_by_random)", res);
	CU_ASSERT(res > -1); /* if we take 100 from 200.. */
}

void test_randomize_in_place(void)
{
	const int sufficient = 100;
	int res = -1;

	// testee
	res = randomize_in_place(candidates, sufficient);
	debug_print("res (randomize_in_place)", res);

	CU_ASSERT(res > -1);
}

void test_online_maximum(void)
{
	int observation_period = ARR_SIZE / 2;
 	const int sufficient = 100;
	int res = -1;

	// testee
	res = on_line_maximum_hiring(candidates, observation_period, sufficient);
	debug_print("res (online maximum)", res);
}

int main()
{
	CU_pSuite pSuite = NULL;

	/* initialize the CUnit test registry */
	if (CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();

	/* add a suite to the registry */
	pSuite = CU_add_suite("utilities", init_suite_hire_assistant,
			      clean_suite_hire_assistant);
	if (NULL == pSuite) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	/* add the tests to the suite */
	TEST_start(pSuite, "hire assistant", test_hire_assistant)
		TEST_append(pSuite, "randomized hire_assistant", test_randomized_hire_assistant)
		TEST_append(pSuite, "permute-by-sorting", test_permute_by_sorting)
		TEST_append(pSuite, "randomize in place", test_randomize_in_place)
		TEST_append(pSuite, "on-line maximum", test_online_maximum)
	TEST_end();

	CU_basic_set_mode(CU_BRM_VERBOSE);
#if defined BASICTEST
	CU_basic_run_tests();
#else
	CU_curses_run_tests();
#endif
	fprintf(stderr, "\n");
	CU_basic_show_failures(CU_get_failure_list());
	fprintf(stderr, "\n\n");

	/* clean up registry and return */
	CU_cleanup_registry();
	return CU_get_error();
}
