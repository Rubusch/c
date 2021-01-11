#ifndef DEVKIT_TEST
#define DEVKIT_TEST

/*
  some smoke tests for my snippet collection

  ---
  References:
  The GNU m4 project and its functional tests helped me to find something useful here.
*/


// test helpers

#define TEST__BEGIN(FUNC)					\
	void test__##FUNC()					\
	{							\
	char test_res[8] = "TODO";				\
	fprintf(stdout, "executing test__%s()...", #FUNC);


#define TEST__END					\
	fprintf(stdout, "%s\n", test_res);		\
	}

#define TEST__OK strcpy(test_res, "OK");

int is_fd_open(int fd)
{
	return (0 <= fcntl(fd, F_GETFL)); // avoid function calls inside macros
//	int res = (0 <= fcntl(fd, F_GETFL)); // avoid function calls inside macros
//	return 0 <= res && (res & FD_CLOEXEC) == 0;
}


#endif /* DEVKIT_TEST */
