// execve.c
/*
  @author: Lothar Rubusch
  @email: L.Rubusch@gmx.ch
  @license: EPL
  @2013-october-22



  #include <unistd.h>
  int execve(const char* path, char* const argv[], char* const envp[]);

  path
      the path to the program to execute
  argv
      a list of argumentes to the program to execute
  envp
      envirionment variables

  Executes a new programm from out of this program.

  The function sets a list of environmental variables.
  e.g. char* env[] = {"USER=user", "PATH=/usr/bin:/bin:/opt/bin", (char*) 0};

  The function returns an error code (0=Ok / -1=Fail).
//*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUF_SIZ 1024
int readFile(char filename[FILENAME_MAX], char ***listOfEnvVars,
	     int *numberOfEnvVars)
{
	if (0 == strlen(filename))
		return -1;

	// init
	FILE *fp = NULL;
	unsigned int alloc_size = BUF_SIZ;
	unsigned int idxStr = 0, numberOfElements;
	char **tmpList;
	char *tmpStr = NULL;

	if (NULL == (tmpStr = calloc(alloc_size, sizeof(*tmpStr)))) {
		perror("calloc() failed");
		return -1;
	}
	memset(tmpStr, '\0', alloc_size);

	// read file
	if (NULL == (fp = fopen(filename, "r"))) {
		perror("fopen() failed");
		return -1;
	}
	int c = 0;
	while (EOF != (c = fgetc(fp))) {
		// separate element
		if ((c == '\n') || (c == '\0')) {
			tmpStr[idxStr] = '\0';
			tmpList = NULL;
			if (NULL ==
			    (tmpList = realloc(*listOfEnvVars,
					       (numberOfElements + 1)))) {
				if (NULL != tmpList)
					free(tmpList);
				tmpList = NULL;
				perror("realloc() failed");
				return -1;
			}
			*listOfEnvVars = tmpList;
			(*listOfEnvVars)[numberOfElements] = tmpStr;
			++numberOfElements; // here size!

			// reset tmpStr here
			tmpStr = NULL;
			if (NULL ==
			    (tmpStr = calloc(BUF_SIZ, sizeof(*tmpStr)))) {
				perror("calloc() failed");
				return -1;
			}
			memset(tmpStr, '\0', BUF_SIZ);
			idxStr = 0;

		} else {
			tmpStr[idxStr] = c;

			// increment
			++idxStr;

			// realloc
			if (idxStr >= alloc_size - 2) {
				char *tmp = NULL;
				if (NULL ==
				    (tmp = realloc(
					     tmpStr,
					     (idxStr + 1 +
					      BUF_SIZ)))) { // FIXME: usage of realloc buggy?
					if (NULL != tmp)
						free(tmp);
					tmp = NULL;
					perror("realloc() failed");
					return -1;
				}
				tmpStr = tmp;
				++alloc_size;
				alloc_size += BUF_SIZ;
			}
		}
	}
	fclose(fp);
	*numberOfEnvVars = numberOfElements;
	return 0;
}

void printError(int errnum)
{
	puts("error handling");

	switch (errnum) {
#ifdef EACCES
	case EACCES:
		perror("EACCES permission denied");
		break;
#endif
#ifdef EPERM
	case EPERM:
		perror("EPERM is not super-user");
		break;
#endif
#ifdef E2BIG
	case E2BIG:
		perror("E2BIG arg list is too long");
		break;
#endif
#ifdef ENOEXEC
	case ENOEXEC:
		perror("ENOEXEC exec format error");
		break;
#endif
#ifdef EFAULT
	case EFAULT:
		perror("EFAULT bad address");
		break;
#endif
#ifdef ENAMETOOLONG
	case ENAMETOOLONG:
		perror("ENAMETOOLONG path name is too long");
		break;
#endif
#ifdef ENOENT
	case ENOENT:
		perror("ENOENT no such file or directory");
		break;
#endif
#ifdef ENOMEM
	case ENOMEM:
		perror("ENOMEM not enough core");
		break;
#endif
#ifdef ENOTDIR
	case ENOTDIR:
		perror("ENOTDIR not a directory");
		break;
#endif
#ifdef ELOOP
	case ELOOP:
		perror("ELOOP too many symbolic links");
		break;
#endif
#ifdef ETXTBSY
	case ETXTBSY:
		perror("ETXTBSY text file busy");
		break;
#endif
#ifdef EIO
	case EIO:
		perror("EIO I/O error");
		break;
#endif
#ifdef ENFILE
	case ENFILE:
		perror("ENFILE too many open files in system");
		break;
#endif
#ifdef EINVAL
	case EINVAL:
		perror("EINVAL invalid argument");
		break;
#endif
#ifdef EISDIR
	case EISDIR:
		perror("EISDIR is a directory");
		break;
#endif
#ifdef ELIBBAD
	case ELIBBAD:
		perror("accessing a corrupted shared lib");
		break;
#endif
	default:
		fprintf(stderr, "error %i occured\r\n", errnum);
		break;
	}
}

void freeMemory(char ***ppList, const unsigned int size)
{
	unsigned int idx = 0;
	for (idx = 0; idx < size; ++idx) {
		if (NULL != (*ppList)[idx])
			free((*ppList)[idx]);
	}
	if (NULL != *ppList)
		free(*ppList);
}

int main()
{
	char envFile[] = "environmental_variables.conf";
	char **listOfEnvVars = NULL;
	int sizeOfEnvVars = 0;
	if (0 != readFile(envFile, &listOfEnvVars, &sizeOfEnvVars)) {
		perror("readFile() failed");
		exit(1);
	}
	pid_t pid = 0;

	// command
	char *listOfArgs[] = { "/bin/ls", "-l", "-a", (char *)0 };

	// fork()
	if (0 > (pid = fork())) {
		perror("fork() failed");
		exit(1);

	} else if (0 == pid) {
		// child code
		int execReturn =
			execve(listOfArgs[0], listOfArgs, listOfEnvVars);

		// function call to execve() does not return, else failure
		perror("Failure!");
		printError(execReturn);
		freeMemory(&listOfEnvVars, sizeOfEnvVars);
		exit(1);

	} else {
		// parent code
		int status = 0;
		while (0 == wait(&status))
			;
		freeMemory(&listOfEnvVars, sizeOfEnvVars);
		exit(0);
	}
}
