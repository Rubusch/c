/*
  setting breakpoints

  init the child with PTRACE_TRACEME and start external program,
  then attach a parent process by declaring PTRACE_ATTACH,
  it requests registers and instruction, prints it and,
  when done, detaches with PTRACE_DETACH

  usage:
  1) start dummy
  $ ./rabbit.exe &
  [1] 14204

  2) start ptracer with obtained pid
  $ ./ptrace.exe 14204


  author: Lothar Rubusch
  email: L.Rubusch@gmx.ch
  original: Linux Journal, Nov 30, 2002  By Pradeep Padala ppadala@cise.ufl.edu
*/

/*
  function to find free space for code injection via map from /proc filesystem
 */
long freespaceaddr(pid_t pid)
{
  FILE *fp;
  char filename[30];
  char line[85];
  long addr;
  char str[20];
  sprintf(filename, "/proc/%d/maps", pid);
  fp = fopen(filename, "r");
  if(fp == NULL)
    exit(1);
  while(fgets(line, 85, fp) != NULL) {
    sscanf(line, "%lx-%*lx %*s %*s %s", &addr,
           str, str, str, str);
    if(strcmp(str, "00:00") == 0)
      break;
  }
  fclose(fp);
  return addr;
}


int main(int argc, char *argv[])
{   pid_t traced_process;
  struct user_regs_struct oldregs, regs;
  long ins;
  int len = 41;
    char insertcode[] =
"\xeb\x15\x5e\xb8\x04\x00"
        "\x00\x00\xbb\x02\x00\x00\x00\x89\xf1\xba"
        "\x0c\x00\x00\x00\xcd\x80\xcc\xe8\xe6\xff"
        "\xff\xff\x48\x65\x6c\x6c\x6f\x20\x57\x6f"
      "\x72\x6c\x64\x0a\x00";
    char backup[len];
    long addr;
    if(argc != 2) {
      printf("Usage: %s <pid to be traced>\n",
             argv[0], argv[1]);
      exit(1);
    }
    traced_process = atoi(argv[1]);
    ptrace(PTRACE_ATTACH, traced_process,
           NULL, NULL);
    wait(NULL);
    ptrace(PTRACE_GETREGS, traced_process,
           NULL, &regs);
    addr = freespaceaddr(traced_process);
    getdata(traced_process, addr, backup, len);
    putdata(traced_process, addr, insertcode, len);
    memcpy(&oldregs, &regs, sizeof(regs));
    regs.eip = addr;
    ptrace(PTRACE_SETREGS, traced_process,
           NULL, &regs);
    ptrace(PTRACE_CONT, traced_process,
           NULL, NULL);
    wait(NULL);
    printf("The process stopped, Putting back "
           "the original instructions\n");
    putdata(traced_process, addr, backup, len);
    ptrace(PTRACE_SETREGS, traced_process,
           NULL, &oldregs);
    printf("Letting it continue with "
           "original flow\n");
    ptrace(PTRACE_DETACH, traced_process,
           NULL, NULL);
    return 0;
}
