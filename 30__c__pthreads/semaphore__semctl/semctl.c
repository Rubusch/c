// semctl.c
/*
  semaphores testing grounds - exerciser for the semctl()
//*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <time.h>

#include "console_input.h"

#define SEM_ID_DIGITS 5
#define SEM_NUM_DIGITS 5
#define SEM_UN_VAL_DIGITS 5
#define SEM_PERM_UID_DIGITS 4
#define SEM_PERM_GID_DIGITS 4
#define SEM_PERM_MODE_DIGITS 4
#define SEM_UN_ARRAY_DIGITS 4

struct semid_ds sem_id_ds;
union semun {
	unsigned int val;
	struct semid_ds *buf;
	unsigned short *array;
};

static void do_semctl();
static void do_stat();

int main(int argc, char **argv)
{
	puts("semctl() exerciser\n");

	// sem_id
	unsigned int sem_id = 0;
	puts("sem_id: the sem_id has to be smaller than the key value");
	readnumber(&sem_id, SEM_ID_DIGITS, "enter a semaphore id value:");

	// sem_cmd
	puts("valid semctl command values are:");
	fprintf(stderr, "0\tGETALL (%d)\n", GETALL);
	fprintf(stderr, "1\tGETNCNT (%d)\n", GETNCNT);
	fprintf(stderr, "2\tGETPID (%d)\n", GETPID);
	fprintf(stderr, "3\tGETVAL (%d)\n", GETVAL);
	fprintf(stderr, "4\tGETZCNT (%d)\n", GETZCNT);
	fprintf(stderr, "5\tIPC_RMID (%d)\n", IPC_RMID);
	fprintf(stderr, "6\tIPC_SET (%d)\n", IPC_SET);
	fprintf(stderr, "7\tIPC_STAT (%d)\n", IPC_STAT);
	fprintf(stderr, "8\tSETALL (%d)\n", SETALL);
	fprintf(stderr, "9\tSETVAL (%d)\n", SETVAL);
	unsigned int chosen = 0;
	unsigned int sem_cmd = 0;
	unsigned int sem_num = 0;
	union semun sem_un;

	readdigit(&chosen, "select a semaphore command");

	switch (chosen) {
	case 0:
		puts("GETALL - get and print the values of all semaphores in the set");
		sem_cmd = GETALL;
		// xxx
		sem_un.buf = &sem_id_ds;
		do_semctl(sem_id, 0, IPC_STAT, sem_un);
		if (NULL == (sem_un.array = calloc(sem_id_ds.sem_nsems,
						   sizeof(*(sem_un.array))))) {
			perror("calloc failed");
			exit(EXIT_FAILURE);
		}
		// xxx
		do_semctl(sem_id, 0, sem_cmd, sem_un);
		{
			register unsigned int idx = 0;
			fprintf(stderr,
				"the values of the %ld semaphores are:\n",
				sem_id_ds.sem_nsems);
			for (idx = 0; idx < sem_id_ds.sem_nsems; ++idx) {
				fprintf(stderr, "%d ", sem_un.array[idx]);
			}
			fprintf(stderr, "\n");
		}
		break;

	case 1:
		puts("GETNCNT - get number of processes waiting for semaphore value to "
		     "increas");
		sem_cmd = GETNCNT;
		// xxx
		readnumber(&sem_num, SEM_NUM_DIGITS,
			   "enter a semaphore number");
		// xxx
		sem_un.val = 0;
		do_semctl(sem_id, sem_num, sem_cmd, sem_un);
		break;

	case 2:
		puts("GETPID - get PID of last process to successfully complete a "
		     "semctl(SETVAL), semctl(SETALL), or semop() on the semaphore");
		sem_cmd = GETPID;
		// xxx
		sem_un.val = 0;
		do_semctl(sem_id, 0, sem_cmd, sem_un);
		break;

	case 3:
		puts("GETVAL - get value of one semaphore");
		sem_cmd = GETVAL;
		// xxx
		readnumber(&sem_num, SEM_NUM_DIGITS,
			   "enter a semaphore number");
		// xxx
		sem_un.val = 0;
		do_semctl(sem_id, sem_num, sem_cmd, sem_un);
		break;

	case 4:
		puts("GETZCNT - get number of processes waiting for semaphore value to "
		     "become zero");
		sem_cmd = GETZCNT;
		// xxx
		readnumber(&sem_num, SEM_NUM_DIGITS,
			   "enter a semaphore number");
		// xxx
		sem_un.val = 0;
		do_semctl(sem_id, sem_num, sem_cmd, sem_un);
		break;

	case 5:
		puts("IPC_RMID - remove the semaphore set");
		sem_cmd = IPC_RMID;
		// xxx
		sem_un.val = 0;
		do_semctl(sem_id, 0, sem_cmd, sem_un);
		break;

	case 6:
		puts("IPC_SET - modify mode and/or ownership");
		sem_cmd = IPC_SET;
		// xxx
		sem_un.buf = &sem_id_ds;
		do_semctl(sem_id, 0, IPC_STAT, sem_un);
		fprintf(stderr, "status before IPC_SET:\n");
		do_stat();
		readnumber(&sem_id_ds.sem_perm.uid, SEM_PERM_UID_DIGITS,
			   "enter sem_perm.uid value:");
		readnumber(&sem_id_ds.sem_perm.gid, SEM_PERM_GID_DIGITS,
			   "enter sem_perm.gid value:");
		readnumber((unsigned int *)&sem_id_ds.sem_perm.mode,
			   SEM_PERM_MODE_DIGITS, "enter sem_perm.mode value:");
		do_semctl(sem_id, 0, sem_cmd, sem_un);
		fprintf(stderr, "status after IPC_SET:\n");
		// xxx
		sem_un.buf = &sem_id_ds;
		do_semctl(sem_id, 0, IPC_STAT, sem_un);
		fprintf(stderr, "status");
		do_stat();
		break;

	case 7:
		puts("IPC_STAT - get and print current status");
		sem_cmd = IPC_STAT;
		// xxx
		sem_un.buf = &sem_id_ds;
		do_semctl(sem_id, 0, sem_cmd, sem_un);
		fprintf(stderr, "status");
		do_stat();
		break;

	case 8:
		puts("SETALL - set the values of all semaphores in the set");
		sem_cmd = SETALL;
		// xxx
		sem_un.buf = &sem_id_ds;
		do_semctl(sem_id, 0, IPC_STAT, sem_un);
		if (NULL == (sem_un.array = calloc(sem_id_ds.sem_nsems,
						   sizeof(*(sem_un.array))))) {
			perror("calloc failed - unable to allocate space for sem_id_ds.sem_nsems");
			exit(EXIT_FAILURE);
		}
		// xxx
		{
			register unsigned int idx = 0;
			puts("enter semaphore values:");
			for (idx = 0; idx < sem_id_ds.sem_nsems; ++idx) {
				fprintf(stderr, "sem_un.array[%d] ", idx);
				readnumber((unsigned int *)&sem_un.array[idx],
					   SEM_UN_ARRAY_DIGITS, "value:");
			}
		}
		do_semctl(sem_id, 0, sem_cmd, sem_un);
		// xxx
		do_semctl(sem_id, 0, GETALL, sem_un);
		{
			register unsigned int idx = 0;
			fprintf(stderr,
				"the values of the %ld semaphores are:\n",
				sem_id_ds.sem_nsems);
			for (idx = 0; idx < sem_id_ds.sem_nsems; ++idx) {
				fprintf(stderr, "%d ", sem_un.array[idx]);
			}
			fprintf(stderr, "\n");
		}
		break;

	case 9:
		puts("SETVAL - set value of one semaphore");
		sem_cmd = SETVAL;
		// xxx
		readnumber(&sem_num, SEM_NUM_DIGITS,
			   "enter a semaphore number");
		// xxx
		readnumber(&sem_un.val, SEM_UN_VAL_DIGITS,
			   "enter a semaphore vlaue");
		do_semctl(sem_id, sem_num, sem_cmd, sem_un);
		// xxx
		sem_un.val = 0;
		do_semctl(sem_id, sem_num, GETVAL, sem_un);
		break;

	default:
		puts("enter digit failed");
		sem_un.val = 0;
		do_semctl(sem_id, 0, sem_cmd, sem_un);
		exit(EXIT_FAILURE);
		break;
	}

	puts("READY.");
	if (NULL != sem_un.array)
		free(sem_un.array); // TODO check
	exit(EXIT_SUCCESS);
}

static void do_semctl(int sem_id, int sem_num, int sem_cmd, union semun sem_un)
{
	register int idx = 0;
	fprintf(stderr,
		"calling semctl(sem_id, sem_num, sem_cmd, union sem_un) - semctl(%d, %d, %d, sem_un)\n",
		sem_id, sem_num, sem_cmd);
	switch (sem_cmd) {
	case GETALL:
		fprintf(stderr, "sem_un.array = %#lx)\n",
			(unsigned long)sem_un.array);
		break;

	case IPC_STAT:
	case IPC_SET:
		fprintf(stderr, "sem_un.buf = %#lx)\n",
			(unsigned long)sem_un.buf);
		break;
	case SETALL:
		fprintf(stderr, "sem_un.array = [");
		for (idx = 0; idx < sem_id_ds.sem_nsems;) {
			fprintf(stderr, "%d", sem_un.array[++idx]);
			if (idx < sem_id_ds.sem_nsems)
				fprintf(stderr, ", ");
		}
		fprintf(stderr, "])\n");
		break;
	default:
		fprintf(stderr, "sem_un.val = %d)\n", sem_un.val);
		break;
	}

	if (0 > (idx = semctl(sem_id, sem_num, sem_cmd, sem_un))) {
		perror("semctl failed");
		exit(1);
	} else {
		fprintf(stderr, "semctl returned %d\n", sem_un.val);
	}
}

static void do_stat()
{
	fprintf(stderr, "sem_perm.uid = %d\n", sem_id_ds.sem_perm.uid);
	fprintf(stderr, "sem_perm.gid = %d\n", sem_id_ds.sem_perm.gid);
	fprintf(stderr, "sem_perm.cuid = %d\n", sem_id_ds.sem_perm.cuid);
	fprintf(stderr, "sem_perm.cgid = %d\n", sem_id_ds.sem_perm.cgid);
	fprintf(stderr, "sem_perm.mode = %#o, ", sem_id_ds.sem_perm.mode);
	fprintf(stderr, "access permissions = %#o\n",
		sem_id_ds.sem_perm.mode & 0777);
	fprintf(stderr, "sem_nsems = %ld\n", sem_id_ds.sem_nsems);
	fprintf(stderr, "sem_otime = %s",
		sem_id_ds.sem_otime ? ctime(&sem_id_ds.sem_otime) :
				      "not set\n");
	fprintf(stderr, "sem_ctime = %s", ctime(&sem_id_ds.sem_ctime));
}
