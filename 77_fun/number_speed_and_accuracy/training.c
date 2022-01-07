/*
  training: further distance

  three numbers are presented
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>
#include <time.h>
#include <math.h>

// training environment
static int ntests;
static int ntests_success;
static int answer;
static int answer_expected;

// setting
static int TIMEOUT = 30;

static int MIN = 1;

////////////////////////////////////////////////////////////////////////////////
//
//static int MAX = 5; // debugging
//static int MAX = 10; // easy
//static int MAX = 100; // average
static int MAX = 1000; // hard
//static int MAX = 10000; // hardcore
//
////////////////////////////////////////////////////////////////////////////////

bool kbhit()
{
	struct termios new_io;
	struct termios old_io;
	int ch;
	int old_fun;

	if (-1 == (tcgetattr(STDIN_FILENO, &old_io))) {
		return false;
	}

	new_io = old_io;
	new_io.c_lflag = new_io.c_lflag & ~(ECHO | ICANON);
	if (-1 == (tcsetattr(STDIN_FILENO, TCSANOW, &new_io))) {
		return -1;
	}
	old_fun = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, old_fun);

	ch = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &old_io);
	fcntl(STDIN_FILENO, F_SETFL, old_io);

	if (ch != EOF) {
		ungetc(ch, stdin);
		return true;
	}

	return false;
}

void readdigit(int *pval, const char *comment, int min, int max)
{
	char input[3];
	int val = -1;
	int res = -1;
	do {
		// reset
		memset(input, '\0', sizeof(input)-1);
		puts(comment);

		// read
		fgets(input, 3, stdin);

		// check stdin and clean in case input exceeded the
		if ('\n' == input[1]) {
			// ok - do nothing here

		} else if (('\n' == input[0]) || ('\n' == input[2])) {
			// number too long or input empty - erease
			memset(input, '\0', sizeof(input)-1);

		} else {
			// '\n' still in stdin - clean up
			puts("input too long - will reset");
			memset(input, '\0', sizeof(input)-1);
			while ('\n' != fgetc(stdin))
				;
		}

		// CAUTION: due to '\n' checking the set of '\0' here!
		input[2] = '\0';

		// number?
		res = isdigit(input[0]);
		if (!res) {
			continue;
		}

		// transform
		val = atoi(input);

		if (min > val || max < val) {
			res = 0;
			continue;
		}

	} while (!res);

	*pval = val;
}

// problem setup
#define ARR_SIZE 3

int get_random_number(int lower, int upper)
{
	return (rand() % (upper - lower + 1)) + lower;
}

void shuffle(int array[ARR_SIZE])
{
	int size = ARR_SIZE;
	int tmp;
	int idx, jdx;

	// random shuffle array
//	srand(time(0));
	for (idx = 0; idx < size - 1; idx++) {
		jdx = idx + rand() / (RAND_MAX / (size - idx) + 1);
		tmp = array[jdx];
		array[jdx] = array[idx];
		array[idx] = tmp;
	}
}

int problem()
{
	if (MAX - MIN < 3) {
		fprintf(stderr, "ABORT: MAX and MIN need larger distance\n");
		exit(EXIT_FAILURE);
	}

	int arr[ARR_SIZE]; memset(arr, 0, sizeof(arr)/sizeof(*arr)-1);
	arr[0] = get_random_number(MIN, (MAX-MIN)/2-1);
	arr[2] = get_random_number((MAX-MIN)/2+1, MAX);
	arr[1] = get_random_number(arr[0]+1, arr[2]-1);

	// obtain distances
	int diff_ab = arr[1] - arr[0];
	int diff_bc = arr[2] - arr[1];

	// furthest distance to middle
	int furthest;
	if (diff_ab > diff_bc) {
		furthest = arr[0];
	} else {
		furthest = arr[2];
	}
	shuffle(arr);

	fprintf(stderr, "\t%d %d %d\n", arr[0], arr[1], arr[2]);

	for (int idx = 0; idx < ARR_SIZE; idx++) {
		if (furthest == arr[idx]) {
			return idx;
		}
	}
	return -1;
}

void handle(int sig, siginfo_t *siginfo, void* context)
{
	fprintf(stderr, "timeout...\n");

	// result
	fprintf(stderr, "result: %d out of %d: %1.3f\n", ntests_success, ntests,
		(((double) ntests_success) / ((double) ntests)));
	fprintf(stderr, "READY.\n");

	exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[])
{
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa)-1);
	sa.sa_sigaction = &handle;
	sa.sa_flags = SA_SIGINFO;
	sigaction(SIGALRM, &sa, NULL);

	// ask start
	fprintf(stderr, "start?\n");
	kbhit();
	{
		// clean up dangling LF / CR
		int ch;
		while((ch = getchar()) != '\n' && ch != EOF)
			;
	}

	// setup signal program after timeout
	srand(time(0));
	ntests = 0;
	ntests_success = 0;
	alarm(TIMEOUT);
	do {
		ntests++;

		// present four pairs of letters
		fprintf(stderr, "\nwhich is the furthest number from middle?\n");
		answer_expected = problem();

		// get input [0,1,2]
		answer = -1;
		readdigit(&answer, "enter position [1,2,3]", 1, 3);
		answer--;

		// feedback
		if (answer == answer_expected) {
			ntests_success++;
		} else {
			fprintf(stderr, "\tWRONG ANSWER -> %d [%d]\n",
				(answer+1), (answer_expected+1));
		}
	} while(true);
	exit(EXIT_SUCCESS);
}
