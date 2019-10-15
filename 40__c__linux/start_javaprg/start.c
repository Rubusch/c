
// start.c

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void startexternal(char *processname, int nargs, ...)
{
  va_list argp;
  int idx = 0;
  char **processargs = NULL;

  processargs = calloc(nargs + 2, sizeof(*processargs));

  // proc name
  processargs[0] = processname;

  // args
  va_start(argp, nargs);
  for (idx = 0; idx < nargs; ++idx) {
    processargs[idx + 1] = ( char * )va_arg(argp, const char *);
  }

  // NULL terminated...
  processargs[idx + 1] = ( char * )0x0;
  va_end(argp);

  // exec
  if (-1 == execvp(processname, processargs)) {
    char _errmsg[256];
    memset(_errmsg, '\0', 256);
    strerror_r(errno, _errmsg, 256);
    fprintf(stderr, "execution failed: \"%s\"\n", _errmsg);
  }

  // free
  free(processargs);
}


int main(int argc, char **args)
{
  char *processname = strdup("java");

  startexternal(processname, 2, "-jar", "./Hello.jar");

  return 0;
}
