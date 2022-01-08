#ifndef TEST_CANBUS_H
#define TEST_CANBUS_H


# ifndef TRUE
#  define TRUE 1
# endif

# ifndef FALSE
#  define FALSE 0
# endif


#include "CUnit/Basic.h"
#include "CUnit/Console.h"
#include "CUnit/Automated.h"
//#include "CUnit/CUCurses.h" /* ncurses support */

#include <stdlib.h>
#include <stdio.h>


// Testee is a pure .c file, else headers go here
//#include ""


/* test hooks */
/* hook for: "int main()"
int tester(void);
int testee(void);
#define TESTING_SETUP				\
	return tester();			\
	}					\
						\
	int testee() {				\

*/

/* hook for: "int main(int argc, char* argv[])" */
int tester(int argc, char* argv[]);
int testee(int argc, char* argv[]);
int global_argc;
char** global_argv;
#define TESTING_SETUP					\
	return tester(argc, argv);			\
	}						\
							\
	int testee(int argc, char* argv[]) {		\


/* probe command line arguments */
char testing_probe__ifname[16];
int testing_probe__ifname__switch;
#define TESTING_PROBE__IFNAME						\
	if (testing_probe__ifname__switch) {				\
		strncpy(testing_probe__ifname, ifname, strlen(ifname));		\
		return EXIT_SUCCESS;					\
	}


/*
  stubbing

 use <name>__stubbed() with:
 <name>__mock -> 1/0 turns mocking on/off
 <name>__ret  -> when mocked, the return value to return
*/
#define test__generate_stub(name, rettype, type1, type2, type3)		\
	static int name##__mock;					\
	static rettype name##__ret;					\
									\
	rettype name##__stubbed(type1 a, type2 b, type3 c)		\
	{								\
		return ((name##__mock)					\
			? (name##__ret)					\
			: (name(a, b, c)));				\
	}


/* stubs */
#define socket socket__stubbed
#define ioctl ioctl__stubbed
#define bind bind__stubbed
#define write write__stubbed
#define close(a)
#define exit(a) return a;

/* positive tests */
int init_suite_success(void);
int clean_suite_success(void);

/* test functions go here... */
void test__socket__failure(void);
void test__ioctl__failure(void);
void test__bind__failure(void);
void test__write__failure(void);
void test__write__success(void);
void test__ifname(void);

#endif /* TEST_CANBUS_H */
