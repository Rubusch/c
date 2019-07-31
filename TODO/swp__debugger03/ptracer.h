/*
  author: Lothar Rubusch
  email: L.Rubusch@gmx.ch
 */
// TODO implementation for JNI code missing

#ifndef PTRACER_H_
#define PTRACER_H_

/* debugging */
#define PARENT_TXT "parent - "
#define CHILD_TXT "\tchild - "
// TODO not used here
//#define IDENTIFIER_SIZE 20

/* technical */
#define TRAP_INST 0xCC
#define TRAP_LEN 1

#if defined(__x86_64)
/* 64bit */
#define REGISTER_IP RIP
#define TRAP_MASK 0xFFFFFFFFFFFFFF00

#elif defined(__i386)
/* 32bit */
#define REGISTER_IP EIP
#define TRAP_MASK 0xFFFFFF00

#else
#error ARCHITECTURE NOT SUPPORTED
#endif

/* utils */
typedef void *tracee_addr_t;

typedef struct breakpoint {
	tracee_addr_t code_addr;
	long code_backup;
} breakpoint_t;

// TODO in case typedef to a breakpoint_p type
typedef breakpoint_t* breakpoint_p;


/* declarations */
struct breakpoint_t;

void printError(int errnum);

// TODO still not used
//void suspend_handler(int signum); 

// TODO still not used
//void resume_handler(int signum); 

void tracee(char** prog, pid_t pid_parent, void (*traceme_fun) (void));
int tracer(pid_t pid);

static void inf_trace_me();
pid_t fork_inferior(char** prog, void (*traceme_fun) (void) );
void startup_inferior( int ntraps );
void inf_ptrace_mourn_inferior();
static void inf_ptrace_create_inferior(char** prog);

breakpoint_t* set_breakpoint(pid_t pid, tracee_addr_t addr);

static int wait_process(const pid_t pid, int *const statusptr);
static int continue_process(const pid_t pid, int *const statusptr);


#endif /* PTRACER_H_ */
