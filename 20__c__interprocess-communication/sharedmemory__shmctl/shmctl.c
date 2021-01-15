// shmctl.c
/*
  shared memory testing grounds - shmctl()
*/

#define _XOPEN_SOURCE 600

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <time.h>

#define ID_SIZ 5

static void do_shmctl();

int isnumber(const char *, const unsigned int);
void readnumber(unsigned int *, const unsigned int, const char *);
void readdigit(unsigned int *, const char *);


int main()
{
	// shm_id
	unsigned int shm_id;
	readnumber(&shm_id, ID_SIZ, "enter a shared memory id\n");
	struct shmid_ds shm_id_ds;

	// command
	int shm_cmd;
	{
		char tmp[3];
		unsigned int chosen;
		do {
			memset(tmp, '\0', 3);
			chosen = 0;
			shm_cmd = 0;

			puts("commands:");
			printf("1\tIPC_RMID\t- %d\n", IPC_RMID);
			printf("2\tIPC_SET \t- %d\n", IPC_SET);
			printf("3\tIPC_STAT\t- %d\n", IPC_STAT);
			printf("4\tSHM_LOCK\t- %d\n", SHM_LOCK);
			printf("5\tSHM_UNLOCK\t- %d\n", SHM_UNLOCK);

			readdigit(&chosen, "enter a command");

			switch (chosen) {
			case 1:
				puts("IPC_RMID");
				shm_cmd = IPC_RMID;
				break;

			case 2:
				puts("IPC_SET");
				shm_cmd = IPC_SET;
				break;

			case 3:
				puts("IPC_STAT");
				shm_cmd = IPC_STAT;
				break;

			case 4:
				puts("SHM_LOCK");
				shm_cmd = SHM_LOCK;
				break;

			case 5:
				puts("SHM_UNLOCK");
				shm_cmd = SHM_UNLOCK;
				break;

			default:
				puts("try again");
				chosen = 0;
				break;
			}
		} while (!chosen);
	}

	switch (shm_cmd) {
	case IPC_STAT:
		puts("get shared memory segment status");
		break;

	case IPC_SET:
		puts("set owner UID and GID and permissions");
		do_shmctl(shm_id, IPC_STAT, &shm_id_ds);
		// using this flag the shmid_ds can be initialised, especially the
		// contained ipc_perm structure (shm_perm) for the permissions
		break;

	case IPC_RMID:
		puts("remove the segment when the last attach point is detached");
		break;

	case SHM_LOCK:
		puts("lock the shared memory section");
		break;

	case SHM_UNLOCK:
		puts("unlock the shared memory section");
		break;

	default:
		puts("COMMAND SYNTAX ERROR.");
		exit(EXIT_FAILURE);
		break;
	}

	do_shmctl(shm_id, shm_cmd, &shm_id_ds);

	puts("READY.");
	exit(EXIT_SUCCESS);
}


static void do_shmctl(int shm_id, int shm_cmd, struct shmid_ds *buf)
{
	if (buf == NULL)
		return;

	register int result; // hold area in CPU
	printf("calling shmctl(%d, %d, buf)\n", shm_id, shm_cmd);


	if (shm_cmd == IPC_SET) {
		fprintf(stderr, "\tbuf->shm_perm.uid == %d\n", buf->shm_perm.uid);
		fprintf(stderr, "\tbuf->shm_perm.gid == %d\n", buf->shm_perm.gid);
		fprintf(stderr, "\tbuf->shm_perm.mode == %#o\n", buf->shm_perm.mode);
	}

	if (0 > (result = shmctl(shm_id, shm_cmd, buf))) {
		perror("shmctl failed");
		exit(EXIT_FAILURE);
	} else {
		fprintf(stderr, "shmctl returned: %d\n", result);
	}

	if ((shm_cmd != IPC_STAT) && (shm_cmd != IPC_SET)) {
		return;
	}

	// content of current status
	fprintf(stderr, "\ncurrent status:\n");
	fprintf(stderr, "\tshm_perm.uid = %d\n", buf->shm_perm.uid);
	fprintf(stderr, "\tshm_perm.gid = %d\n", buf->shm_perm.gid);
	fprintf(stderr, "\tshm_perm.cuid = %d\n", buf->shm_perm.cuid);
	fprintf(stderr, "\tshm_perm.cgid = %d\n", buf->shm_perm.cgid);
	fprintf(stderr, "\tshm_perm.mode = %#o\n", buf->shm_perm.mode);
	fprintf(stderr, "\tshm_perm.__key = %x\n", buf->shm_perm.__key);
	fprintf(stderr, "\tshm_segsz = %ld\n", buf->shm_segsz);
	fprintf(stderr, "\tshm_lpid = %d\n", buf->shm_lpid);
	fprintf(stderr, "\tshm_cpid = %d\n", buf->shm_cpid);
	//  fprintf(stderr, "\tshm_nattach = %s\n", buf->shm_nattach);
	fprintf(stderr, "\tshm_atime = %s",
		buf->shm_atime ? ctime(&buf->shm_atime) : "not set\n");
	fprintf(stderr, "\tshm_dtime = %s",
		buf->shm_dtime ? ctime(&buf->shm_dtime) : "not set\n");
	fprintf(stderr, "\tshm_ctime = %s", ctime(&buf->shm_ctime));
}


int isnumber(const char *str, const unsigned int size)
{
	char arr[size];
	memset(arr, '\0', size);
	strncpy(arr, str, size);
	arr[size - 1] = '\0';

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


void readdigit(unsigned int *iChr, const char *comment)
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
	*iChr = atoi(cChr);
}
