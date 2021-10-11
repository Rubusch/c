// shm_attachdetach.c
/*
  TODO: runs buggy!

  shared memory testing grounds - attach and detach a process to shm
*/

#define _XOPEN_SOURCE 600

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <setjmp.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#define MAXnap 4

// digits
#define ID_DIGITS 8
#define ADDR_DIGITS 8
#define AREA_DIGITS 4

// internal record of currently attached segments
static struct state {
	// shm id of the attached segment
	unsigned int shm_id;

	// attach pointer
	char *shm_addr;

	// flags used on attach
	unsigned int shm_flag;
} attached_segments[MAXnap];

// number of currently attached segments
static int nap = 0;

// process state save area for SIGSEGV catching
static jmp_buf segvbuf;

// prg funcs
static int ask();
static void signalhandler(int);
static int good_addr(char *);

// reading
int isnumber(const char *, const unsigned int);
void readnumber(unsigned int *, const unsigned int, const char *);
void readstring(char *, const unsigned int, const char *);
void readdigit(unsigned int *, const char *);

int main(int argc, char **argv)
{
	// in case generate shared memory to pointer area_addr
	// address work area
	char *area_addr = NULL;

	// work area
	register int area_id = 0;

	// ptr to current state entry
	register struct state *pState = NULL;

	// SIGSEGV state hold area func ptr
	void (*savefunc)();

	// action to be performed
	register int action = 0;
	while (0 != (action = ask())) {
		if (0 < nap) {
			fprintf(stderr, "currently attached segments");
			fprintf(stderr, " shm_id address\n");
			fprintf(stderr, "------- ---------\n");
			pState = &attached_segments[nap + 1];
			while (--pState != attached_segments) {
				fprintf(stderr, "%6d", pState->shm_id);
				fprintf(stderr, "%#11x",
					*(unsigned int *)pState->shm_addr);
				fprintf(stderr, " Read%s\n",
					(pState->shm_flag & SHM_RDONLY) ?
						"-Only" :
						"/Write");
			}
			fprintf(stderr, "\n");

		} else {
			fprintf(stderr, "no segments are currently attached\n");
			switch (action) {
			case 1:
				// 1 - verify that there is space for another attach
				if (nap == MAXnap) {
					fprintf(stderr,
						"this simple example will only allow %d attached segments\n",
						MAXnap);
					break;
				}
				// get the arguments, make the call, report the result and update the
				// current state array
				pState = &attached_segments[++nap];

				readnumber(
					&pState->shm_id, ID_DIGITS,
					"enter shm_id of segment to attach:");
				readstring(pState->shm_addr, ADDR_DIGITS,
					   "enter shm_addr");
				fprintf(stderr,
					"meaningful shm_flag vlaues are:\n");
				fprintf(stderr, "1\tSHM_RDONLY (%d)\n",
					SHM_RDONLY);
				fprintf(stderr, "2\tSHM_RND (%d)\n", SHM_RND);
				unsigned int select = 0;
				unsigned int askagain = 0;
				do {
					askagain = 0;
					readdigit(&select, "select a shm_flag");
					switch (select) {
					case 1:
						puts("SHM_RDONLY");
						pState->shm_flag = SHM_RDONLY;
						break;

					case 2:
						puts("SHM_RND");
						pState->shm_flag = SHM_RND;
						break;

					default:
						puts("wrong input - try again");
						askagain = 1;
						break;
					}
				} while (askagain);
				fprintf(stderr,
					"now attach the segment - shmat(%d, %#x, %#o)\n",
					pState->shm_id,
					*(unsigned int *)pState->shm_addr,
					pState->shm_flag);

				if (0 >
				    *(int8_t *)(pState->shm_addr = shmat(
							pState->shm_id,
							pState->shm_addr,
							pState->shm_flag))) {
					perror("shmat() failed");
					--nap; // decrement, because already incremented above, for new
					// pointer element!
				} else {
					fprintf(stderr,
						"shmat returned %#8.8x\n",
						*(int8_t *)pState->shm_addr);
				}
				break;

			case 2:
				// 2 - shmdt requested
				/*
				  get the address, make the call, report the results and make the
				  internal state match
				//*/
				readstring(area_addr, ADDR_DIGITS,
					   "enter detach shm_addr");
				if (-1 == (area_id = shmdt(area_addr))) {
					perror("shmdt failed");
				} else {
					fprintf(stderr, "shmdt returned %d\n",
						area_id);
					for (pState = attached_segments,
					    area_id = nap;
					     --area_id; ++pState) {
						if (0 != (pState->shm_addr =
								  area_addr))
							*pState =
								attached_segments
									[--nap];
					}
				}
				break;

			case 3:
				if (nap == 0)
					break;

				readstring(
					area_addr, ADDR_DIGITS,
					"enter address of an attached segment");
				if (good_addr(area_addr))
					fprintf(stderr,
						"string @ %#x is '%s'\n",
						*(int8_t *)area_addr,
						area_addr);
				break;

			case 4:
				if (nap == 0)
					break;
				readstring(
					area_addr, ADDR_DIGITS,
					"enter address of an attached segment");

				/*
				  set up SIGSEGV signal handler to trap attempts to write into a
				  read-only attached segment
				//*/
				savefunc = signal(SIGSEGV, signalhandler);

				if (setjmp(segvbuf)) {
					fprintf(stderr,
						"SIGSEGV signal caught: write aborted\n");
					fflush(stdin);
					signal(SIGSEGV, savefunc);
				} else {
					if (good_addr(area_addr)) {
						fflush(stdin);
						readstring(
							area_addr, AREA_DIGITS,
							"enter one line to e copied to the shared segment attached");
						// TODO: check
						if (NULL ==
						    strncpy(pState->shm_addr,
							    (char *)-1,
							    sizeof(char *))) {
							perror("strncpy to shared memory failed");
							break;
						}
					}
				}
				break;
			}
		}
	}

	// not reached
	puts("READY.");
	exit(EXIT_SUCCESS);
}

static int ask()
{
	unsigned int response = 0;
	do {
		response = 0;
		fprintf(stderr, "your options are:\n");
		fprintf(stderr, "0\texit\n");
		fprintf(stderr, "1\tshmat\n");
		fprintf(stderr, "2\tshmdt\n");
		fprintf(stderr, "3\tread from segment\n");
		fprintf(stderr, "4\twrite to segment\n");
		readdigit(&response,
			  "enter the number corresponding to your choice");
	} while ((0 > response) || (4 < response));
	return response;
}

static void signalhandler(int dummy)
{
	longjmp(segvbuf, 1);
	// unreached
}

static int good_addr(char *address)
{
	register struct state *pState = NULL;
	for (pState = attached_segments; pState != &attached_segments[nap];
	     ++pState)
		if (0 == strncmp(pState->shm_addr, address, ADDR_DIGITS))
			return 1;
	return 0;
}

// TOOLS
int isnumber(const char *szNum, const unsigned int szNum_size)
{
	char arr[szNum_size];
	memset(arr, '\0', szNum_size);
	strncpy(arr, szNum, szNum_size);
	arr[szNum_size - 1] = '\0';

	const unsigned int len = strlen(arr);
	if (0 == len)
		return 0;

	int idx;
	for (idx = 0; idx < len; ++idx) {
		if (('\n' == arr[idx]) || ('\0' == arr[idx]))
			break;
		if (!isdigit(arr[idx]))
			return 0;
	}
	return 1;
}

void readnumber(unsigned int *iVal, const unsigned int digits,
		const char *comment)
{
	if (NULL == comment) {
		perror("text is NULL");
		return;
	}
	if (NULL == iVal) {
		perror("iVal is NULL");
		return;
	}

	unsigned int size = digits + 1;
	char cTxt[size];
	do {
		// reset
		memset(cTxt, '\0', size);
		puts(comment);

		// read in
		unsigned int c;
		unsigned int idx = 0;

		// in case of exceeding the size of the variable - put a '\0' at the end
		// and read until '\n', but don't store the characters for cleaning the
		// stream
		for (idx = 0; ('\n' != (c = fgetc(stdin))); ++idx) {
			if ((size - 1) > idx)
				cTxt[idx] = c;
			else if ((size - 1) == idx) {
				puts("input too long - will be reset");
				memset(cTxt, '\0', size);
				// or cut here:
				// cTxt[idx] = '\0';
			}
		}
	} while (!isnumber(cTxt, (1 + strlen(cTxt))));
	*iVal = atoi(cTxt);
}

void readstring(char *cTxt, const unsigned int txtSize, const char *comment)
{
	if (NULL == comment) {
		perror("text is NULL");
		return;
	}
	if (NULL == cTxt) {
		perror("iTxt is NULL");
		return;
	}

	do {
		// reset
		memset(cTxt, '\0', txtSize);
		puts(comment);

		// read in
		unsigned int c;
		unsigned int idx = 0;

		// in case of exceeding the size of the variable - put a '\0' at the end
		// and read until '\n', but don't store the characters for cleaning the
		// stream
		for (idx = 0; ('\n' != (c = fgetc(stdin))); ++idx) {
			if ((txtSize - 1) > idx)
				cTxt[idx] = c;
			else if ((txtSize - 1) == idx) {
				puts("input too long - will be reset");
				memset(cTxt, '\0', txtSize);
				// or cut here:
				// cTxt[idx] = '\0';
			}
		}

	} while (0 == strlen(cTxt));
}

void readdigit(unsigned int *iVal, const char *comment)
{
	char cChr[3];
	do {
		// reset
		memset(cChr, '\0', 3);
		puts(comment);

		// read
		fgets(cChr, 3, stdin);

		// check stdin and clean in case input exceeded the
		if ('\n' == cChr[1]) {
			// ok - do nothing here

		} else if (('\n' == cChr[0]) || ('\n' == cChr[2])) {
			// number too long or input empty - erease
			memset(cChr, '\0', 3);

		} else {
			// '\n' still in stdin - clean up
			puts("input too long - will reset");
			memset(cChr, '\0', 3);
			while ('\n' != fgetc(stdin))
				;
		}

		// CAUTION: due to '\n' checking the set of '\0' here!
		cChr[2] = '\0';

		// number?
	} while (!isdigit(cChr[0]));

	// transform
	*iVal = atoi(cChr);
}
