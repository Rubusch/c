/*
  author: Lothar Rubusch
  email: L.Rubusch@gmx.ch
 */

#ifndef PTRACER_H_
#define PTRACER_H_

/* debugging */
#define PARENT_TXT "parent - "
#define CHILD_TXT "\tchild - "
//#define IDENTIFIER_SIZE 20


/* technical */
#define TRAP_INST 0xCC
#define TRAP_LEN 1

#if defined(__x86_64)
#define REGISTER_IP RIP
#define TRAP_MASK 0xFFFFFFFFFFFFFF00

#elif defined(__i386)
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


/* declarations */
struct breakpoint_t;

void printError(int errnum);
void suspend_handler(int signum); 
void resume_handler(int signum); 
void child( pid_t pid_parent, void (*traceme_fun) (void) );
int tracer(pid_t pid);
static void inf_trace_me();
pid_t fork_inferior( void (*traceme_fun) (void) );
void startup_inferior( int ntraps );
void inf_ptrace_mourn_inferior();
static void inf_ptrace_create_inferior( );
static void enable( pid_t pid, breakpoint_t *bp);
int iteration_run(pid_t pid, breakpoint_t *bp);
//static int run(pid_t pid, int cmd);  
tracee_addr_t get_main_address(pid_t pid);
static int wait_process(const pid_t pid, int *const statusptr);

#endif /* PTRACER_H_ */
