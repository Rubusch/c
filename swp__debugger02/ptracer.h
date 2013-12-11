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
#define TRAP_LENGTH 1

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



#endif /* PTRACER_H_ */
