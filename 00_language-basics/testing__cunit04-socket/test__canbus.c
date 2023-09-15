/*
  demo - how to test e.g. a socket implementation via Cunit

  @author: Lothar Rubusch <l.rubusch@gmail.com>
  @license: GPLv3
*/

#include "test__canbus.h"

static int global_argc;
static char** global_argv;

int init_suite_success(void) { return 0; }
int clean_suite_success(void) { return 0; }

test__generate_stub(socket, int, int, int, int);
test__generate_stub(ioctl, int, int, int, int);
test__generate_stub(bind, int, int, int, int);
test__generate_stub(write, int, int, int, int);

void test__socket__failure(void)
{
	socket__mock = 1;
	socket__ret = -1;
	CU_ASSERT(EXIT_FAILURE == testee(global_argc, global_argv));
}

void test__ioctl__failure(void)
{
	socket__mock = 1;
	socket__ret = 1;
	ioctl__mock = 1;
	ioctl__ret = -1;
	CU_ASSERT(EXIT_FAILURE == testee(global_argc, global_argv));
}

void test__bind__failure(void)
{
	socket__mock = 1;
	socket__ret = 1;
	ioctl__mock = 1;
	ioctl__ret = 1;
	bind__mock = 1;
	bind__ret = -1;
	CU_ASSERT(EXIT_FAILURE == testee(global_argc, global_argv));
}

void test__write__failure(void)
{
	socket__mock = 1;
	socket__ret = 1;
	ioctl__mock = 1;
	ioctl__ret = 1;
	bind__mock = 1;
	bind__ret = 1;
	write__mock = 1;
	write__ret = -1;
	CU_ASSERT(EXIT_FAILURE == testee(global_argc, global_argv));
}

void test__write__success(void)
{
	socket__mock = 1;
	socket__ret = 1;
	ioctl__mock = 1;
	ioctl__ret = 1;
	bind__mock = 1;
	bind__ret = 1;
	write__mock = 1;
	write__ret = 1;
	CU_ASSERT(EXIT_SUCCESS == testee(global_argc, global_argv));
}

void test__ifname(void)
{
	const int argc_single = 1;
	const int argc = 2;
	char *argv[argc];
	char ifname[32] = "canape";

	/* nothing passed */
	memset(testing_probe__ifname, '\0', 16);
	testing_probe__ifname__switch = 1;
	CU_ASSERT(EXIT_SUCCESS == testee(argc_single, argv));
	CU_ASSERT(0 == strncmp(testing_probe__ifname, "can0", sizeof("can0")));

	/* passed name */
	memset(testing_probe__ifname, '\0', 16);
	argv[1] = ifname;
	testing_probe__ifname__switch = 1;
	CU_ASSERT(EXIT_SUCCESS == testee(argc, argv));
	CU_ASSERT(0 == strncmp(testing_probe__ifname, ifname, sizeof(ifname)));

	/* oversize */
	memset(argv[1], '\0', 32);
	memset(testing_probe__ifname, '\0', 16);
	strcpy(argv[1], "1234567890123456");
	testing_probe__ifname__switch = 1;
	CU_ASSERT(EXIT_SUCCESS == testee(argc, argv));
	CU_ASSERT(0 == strncmp(testing_probe__ifname, "can0", sizeof("can0")));
}

/*
int tester(void)
/*/
int tester(int argc, char* argv[])
// */
{
	global_argc = argc;
	global_argv = argv;
	CU_pSuite pSuite = NULL;

	/* initialize the CUnit test registry */
	if (CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();

	/* add a suite to the registry */
	pSuite = CU_add_suite("Suite_success",
			      init_suite_success,
			      clean_suite_success);
	if (NULL == pSuite) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	/* add the tests to the suite */
	if ((NULL == CU_add_test(pSuite, "socket failure", test__socket__failure))
	    || (NULL == CU_add_test(pSuite, "ioctl failure", test__ioctl__failure))
	    || (NULL == CU_add_test(pSuite, "bind failure", test__bind__failure))
	    || (NULL == CU_add_test(pSuite, "write failure", test__write__failure))
	    || (NULL == CU_add_test(pSuite, "writte success", test__write__success))
	    || (NULL == CU_add_test(pSuite, "test ifname", test__ifname))
/* add :    || (NULL == CU_add_test(pSuite, "test__comment", <test__function>)) */
		) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	/* Run all tests using the basic interface */
	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	printf("\n");
	CU_basic_show_failures(CU_get_failure_list());
	printf("\n\n");

	CU_cleanup_registry();
	return CU_get_error();
}
