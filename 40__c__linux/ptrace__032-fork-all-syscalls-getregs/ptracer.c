/*
  ptrace example: x86_64 only!


  fork child with PTRACE_TRACEME

  let it execute a command, e.g. "/bin/pwd"

  read all syscall registers with PTRACE_GETREGS

  show all syscalls


  ---


  identify a tracee's syscall

  0 (0x0) read sys_read
  1 (0x1) write sys_write
  2 (0x2) open sys_open
  3 (0x3) close sys_close
  4 (0x4) stat sys_newstat
  5 (0x5) fstat sys_fstat
  6 (0x6) lstat sys_lstat
  7 (0x7) poll sys_poll
  8 (0x8) lseek sys_lseek
  9 (0x9) mmap sys_mmap
  10 (0xa) mprotect sys_mprotect
  11 (0xb) munmap sys_munmap
  12 (0xc) brk sys_brk
  13 (0xd) rt_sigaction sys_rt_sigaction
  14 (0xe) rt_sigprocmask sys_rt_sigprocmask
  15 (0xf) rt_sigreturn stub_rt_sigreturn
  16 (0x10) ioctl sys_ioctl
  17 (0x11) pread64 sys_pread64
  18 (0x12) pwrite64 sys_pwrite64
  19 (0x13) readv sys_readv
  20 (0x14) writev sys_writev
  21 (0x15) access sys_access
  22 (0x16) pipe sys_pipe
  23 (0x17) select sys_select
  24 (0x18) sched_yield sys_sched_yield
  25 (0x19) mremap sys_mremap
  26 (0x1a) msync sys_msync
  27 (0x1b) mincore sys_mincore
  28 (0x1c) madvise sys_madvise
  29 (0x1d) shmget sys_shmget
  30 (0x1e) shmat sys_shmat
  31 (0x1f) shmctl sys_shmctl
  32 (0x20) dup sys_dup
  33 (0x21) dup2 sys_dup2
  34 (0x22) pause sys_pause
  35 (0x23) nanosleep sys_nanosleep
  36 (0x24) getitimer sys_getitimer
  37 (0x25) alarm sys_alarm
  38 (0x26) setitimer sys_setitimer
  39 (0x27) getpid sys_getpid
  40 (0x28) sendfile sys_sendfile64
  41 (0x29) socket sys_socket
  42 (0x2a) connect sys_connect
  43 (0x2b) accept sys_accept
  44 (0x2c) sendto sys_sendto
  45 (0x2d) recvfrom sys_recvfrom
  46 (0x2e) sendmsg sys_sendmsg
  47 (0x2f) recvmsg sys_recvmsg
  48 (0x30) shutdown sys_shutdown
  49 (0x31) bind sys_bind
  50 (0x32) listen sys_listen
  51 (0x33) getsockname sys_getsockname
  52 (0x34) getpeername sys_getpeername
  53 (0x35) socketpair sys_socketpair
  54 (0x36) setsockopt sys_setsockopt
  55 (0x37) getsockopt sys_getsockopt
  56 (0x38) clone stub_clone
  57 (0x39) fork stub_fork
  58 (0x3a) vfork stub_vfork
  59 (0x3b) execve stub_execve
  60 (0x3c) exit sys_exit
  61 (0x3d) wait4 sys_wait4
  62 (0x3e) kill sys_kill
  63 (0x3f) uname sys_newuname
  64 (0x40) semget sys_semget
  65 (0x41) semop sys_semop
  66 (0x42) semctl sys_semctl
  67 (0x43) shmdt sys_shmdt
  68 (0x44) msgget sys_msgget
  69 (0x45) msgsnd sys_msgsnd
  70 (0x46) msgrcv sys_msgrcv
  71 (0x47) msgctl sys_msgctl
  72 (0x48) fcntl sys_fcntl
  73 (0x49) flock sys_flock
  74 (0x4a) fsync sys_fsync
  75 (0x4b) fdatasync sys_fdatasync
  76 (0x4c) truncate sys_truncate
  77 (0x4d) ftruncate sys_ftruncate
  78 (0x4e) getdents sys_getdents
  79 (0x4f) getcwd sys_getcwd
  80 (0x50) chdir sys_chdir
  81 (0x51) fchdir sys_fchdir
  82 (0x52) rename sys_rename
  83 (0x53) mkdir sys_mkdir
  84 (0x54) rmdir sys_rmdir
  85 (0x55) creat sys_creat
  86 (0x56) link sys_link
  87 (0x57) unlink sys_unlink
  88 (0x58) symlink sys_symlink
  89 (0x59) readlink sys_readlink
  90 (0x5a) chmod sys_chmod
  91 (0x5b) fchmod sys_fchmod
  92 (0x5c) chown sys_chown
  93 (0x5d) fchown sys_fchown
  94 (0x5e) lchown sys_lchown
  95 (0x5f) umask sys_umask
  96 (0x60) gettimeofday sys_gettimeofday
  97 (0x61) getrlimit sys_getrlimit
  98 (0x62) getrusage sys_getrusage
  99 (0x63) sysinfo sys_sysinfo
  100 (0x64) times sys_times
  101 (0x65) ptrace sys_ptrace
  102 (0x66) getuid sys_getuid
  103 (0x67) syslog sys_syslog
  104 (0x68) getgid sys_getgid
  105 (0x69) setuid sys_setuid
  106 (0x6a) setgid sys_setgid
  107 (0x6b) geteuid sys_geteuid
  108 (0x6c) getegid sys_getegid
  109 (0x6d) setpgid sys_setpgid
  110 (0x6e) getppid sys_getppid
  111 (0x6f) getpgrp sys_getpgrp
  112 (0x70) setsid sys_setsid
  113 (0x71) setreuid sys_setreuid
  114 (0x72) setregid sys_setregid
  115 (0x73) getgroups sys_getgroups
  116 (0x74) setgroups sys_setgroups
  117 (0x75) setresuid sys_setresuid
  118 (0x76) getresuid sys_getresuid
  119 (0x77) setresgid sys_setresgid
  120 (0x78) getresgid sys_getresgid
  121 (0x79) getpgid sys_getpgid
  122 (0x7a) setfsuid sys_setfsuid
  123 (0x7b) setfsgid sys_setfsgid
  124 (0x7c) getsid sys_getsid
  125 (0x7d) capget sys_capget
  126 (0x7e) capset sys_capset
  127 (0x7f) rt_sigpending sys_rt_sigpending
  128 (0x80) rt_sigtimedwait sys_rt_sigtimedwait
  129 (0x81) rt_sigqueueinfo sys_rt_sigqueueinfo
  130 (0x82) rt_sigsuspend sys_rt_sigsuspend
  131 (0x83) sigaltstack sys_sigaltstack
  132 (0x84) utime sys_utime
  133 (0x85) mknod sys_mknod
  134 (0x86) uselib
  135 (0x87) personality sys_personality
  136 (0x88) ustat sys_ustat
  137 (0x89) statfs sys_statfs
  138 (0x8a) fstatfs sys_fstatfs
  139 (0x8b) sysfs sys_sysfs
  140 (0x8c) getpriority sys_getpriority
  141 (0x8d) setpriority sys_setpriority
  142 (0x8e) sched_setparam sys_sched_setparam
  143 (0x8f) sched_getparam sys_sched_getparam
  144 (0x90) sched_setscheduler sys_sched_setscheduler
  145 (0x91) sched_getscheduler sys_sched_getscheduler
  146 (0x92) sched_get_priority_max sys_sched_get_priority_max
  147 (0x93) sched_get_priority_min sys_sched_get_priority_min
  148 (0x94) sched_rr_get_interval sys_sched_rr_get_interval
  149 (0x95) mlock sys_mlock
  150 (0x96) munlock sys_munlock
  151 (0x97) mlockall sys_mlockall
  152 (0x98) munlockall sys_munlockall
  153 (0x99) vhangup sys_vhangup
  154 (0x9a) modify_ldt sys_modify_ldt
  155 (0x9b) pivot_root sys_pivot_root
  156 (0x9c) _sysctl sys_sysctl
  157 (0x9d) prctl sys_prctl
  158 (0x9e) arch_prctl sys_arch_prctl
  159 (0x9f) adjtimex sys_adjtimex
  160 (0xa0) setrlimit sys_setrlimit
  161 (0xa1) chroot sys_chroot
  162 (0xa2) sync sys_sync
  163 (0xa3) acct sys_acct
  164 (0xa4) settimeofday sys_settimeofday
  165 (0xa5) mount sys_mount
  166 (0xa6) umount2 sys_umount
  167 (0xa7) swapon sys_swapon
  168 (0xa8) swapoff sys_swapoff
  169 (0xa9) reboot sys_reboot
  170 (0xaa) sethostname sys_sethostname
  171 (0xab) setdomainname sys_setdomainname
  172 (0xac) iopl stub_iopl
  173 (0xad) ioperm sys_ioperm
  174 (0xae) create_module
  175 (0xaf) init_module sys_init_module
  176 (0xb0) delete_module sys_delete_module
  177 (0xb1) get_kernel_syms
  178 (0xb2) query_module
  179 (0xb3) quotactl sys_quotactl
  180 (0xb4) nfsservctl
  181 (0xb5) getpmsg
  182 (0xb6) putpmsg
  183 (0xb7) afs_syscall
  184 (0xb8) tuxcall
  185 (0xb9) security
  186 (0xba) gettid sys_gettid
  187 (0xbb) readahead sys_readahead
  188 (0xbc) setxattr sys_setxattr
  189 (0xbd) lsetxattr sys_lsetxattr
  190 (0xbe) fsetxattr sys_fsetxattr
  191 (0xbf) getxattr sys_getxattr
  192 (0xc0) lgetxattr sys_lgetxattr
  193 (0xc1) fgetxattr sys_fgetxattr
  194 (0xc2) listxattr sys_listxattr
  195 (0xc3) llistxattr sys_llistxattr
  196 (0xc4) flistxattr sys_flistxattr
  197 (0xc5) removexattr sys_removexattr
  198 (0xc6) lremovexattr sys_lremovexattr
  199 (0xc7) fremovexattr sys_fremovexattr
  200 (0xc8) tkill sys_tkill
  201 (0xc9) time sys_time
  202 (0xca) futex sys_futex
  203 (0xcb) sched_setaffinity sys_sched_setaffinity
  204 (0xcc) sched_getaffinity sys_sched_getaffinity
  205 (0xcd) set_thread_area
  206 (0xce) io_setup sys_io_setup
  207 (0xcf) io_destroy sys_io_destroy
  208 (0xd0) io_getevents sys_io_getevents
  209 (0xd1) io_submit sys_io_submit
  210 (0xd2) io_cancel sys_io_cancel
  211 (0xd3) get_thread_area
  212 (0xd4) lookup_dcookie sys_lookup_dcookie
  213 (0xd5) epoll_create sys_epoll_create
  214 (0xd6) epoll_ctl_old
  215 (0xd7) epoll_wait_old
  216 (0xd8) remap_file_pages sys_remap_file_pages
  217 (0xd9) getdents64 sys_getdents64
  218 (0xda) set_tid_address sys_set_tid_address
  219 (0xdb) restart_syscall sys_restart_syscall
  220 (0xdc) semtimedop sys_semtimedop
  221 (0xdd) fadvise64 sys_fadvise64
  222 (0xde) timer_create sys_timer_create
  223 (0xdf) timer_settime sys_timer_settime
  224 (0xe0) timer_gettime sys_timer_gettime
  225 (0xe1) timer_getoverrun sys_timer_getoverrun
  226 (0xe2) timer_delete sys_timer_delete
  227 (0xe3) clock_settime sys_clock_settime
  228 (0xe4) clock_gettime sys_clock_gettime
  229 (0xe5) clock_getres sys_clock_getres
  230 (0xe6) clock_nanosleep sys_clock_nanosleep
  231 (0xe7) exit_group sys_exit_group
  232 (0xe8) epoll_wait sys_epoll_wait
  233 (0xe9) epoll_ctl sys_epoll_ctl
  234 (0xea) tgkill sys_tgkill
  235 (0xeb) utimes sys_utimes
  236 (0xec) vserver
  237 (0xed) mbind sys_mbind
  238 (0xee) set_mempolicy sys_set_mempolicy
  239 (0xef) get_mempolicy sys_get_mempolicy
  240 (0xf0) mq_open sys_mq_open
  241 (0xf1) mq_unlink sys_mq_unlink
  242 (0xf2) mq_timedsend sys_mq_timedsend
  243 (0xf3) mq_timedreceive sys_mq_timedreceive
  244 (0xf4) mq_notify sys_mq_notify
  245 (0xf5) mq_getsetattr sys_mq_getsetattr
  246 (0xf6) kexec_load sys_kexec_load
  247 (0xf7) waitid sys_waitid
  248 (0xf8) add_key sys_add_key
  249 (0xf9) request_key sys_request_key
  250 (0xfa) keyctl sys_keyctl
  251 (0xfb) ioprio_set sys_ioprio_set
  252 (0xfc) ioprio_get sys_ioprio_get
  253 (0xfd) inotify_init sys_inotify_init
  254 (0xfe) inotify_add_watch sys_inotify_add_watch
  255 (0xff) inotify_rm_watch sys_inotify_rm_watch
  256 (0x100) migrate_pages sys_migrate_pages
  257 (0x101) openat sys_openat
  258 (0x102) mkdirat sys_mkdirat
  259 (0x103) mknodat sys_mknodat
  260 (0x104) fchownat sys_fchownat
  261 (0x105) futimesat sys_futimesat
  262 (0x106) newfstatat sys_newfstatat
  263 (0x107) unlinkat sys_unlinkat
  264 (0x108) renameat sys_renameat
  265 (0x109) linkat sys_linkat
  266 (0x10a) symlinkat sys_symlinkat
  267 (0x10b) readlinkat sys_readlinkat
  268 (0x10c) fchmodat sys_fchmodat
  269 (0x10d) faccessat sys_faccessat
  270 (0x10e) pselect6 sys_pselect6
  271 (0x10f) ppoll sys_ppoll
  272 (0x110) unshare sys_unshare
  273 (0x111) set_robust_list sys_set_robust_list
  274 (0x112) get_robust_list sys_get_robust_list
  275 (0x113) splice sys_splice
  276 (0x114) tee sys_tee
  277 (0x115) sync_file_range sys_sync_file_range
  278 (0x116) vmsplice sys_vmsplice
  279 (0x117) move_pages sys_move_pages
  280 (0x118) utimensat sys_utimensat
  281 (0x119) epoll_pwait sys_epoll_pwait
  282 (0x11a) signalfd sys_signalfd
  283 (0x11b) timerfd_create sys_timerfd_create
  284 (0x11c) eventfd sys_eventfd
  285 (0x11d) fallocate sys_fallocate
  286 (0x11e) timerfd_settime sys_timerfd_settime
  287 (0x11f) timerfd_gettime sys_timerfd_gettime
  288 (0x120) accept4 sys_accept4
  289 (0x121) signalfd4 sys_signalfd4
  290 (0x122) eventfd2 sys_eventfd2
  291 (0x123) epoll_create1 sys_epoll_create1
  292 (0x124) dup3 sys_dup3
  293 (0x125) pipe2 sys_pipe2
  294 (0x126) inotify_init1 sys_inotify_init1
  295 (0x127) preadv sys_preadv
  296 (0x128) pwritev sys_pwritev
  297 (0x129) rt_tgsigqueueinfo sys_rt_tgsigqueueinfo
  298 (0x12a) perf_event_open sys_perf_event_open
  299 (0x12b) recvmmsg sys_recvmmsg
  300 (0x12c) fanotify_init sys_fanotify_init
  301 (0x12d) fanotify_mark sys_fanotify_mark
  302 (0x12e) prlimit64 sys_prlimit64
  303 (0x12f) name_to_handle_at sys_name_to_handle_at
  304 (0x130) open_by_handle_at sys_open_by_handle_at
  305 (0x131) clock_adjtime sys_clock_adjtime
  306 (0x132) syncfs sys_syncfs
  307 (0x133) sendmmsg sys_sendmmsg
  308 (0x134) setns sys_setns
  309 (0x135) getcpu sys_getcpu
  310 (0x136) process_vm_readv sys_process_vm_readv
  311 (0x137) process_vm_writev sys_process_vm_writev
  312 (0x138) kcmp sys_kcmp
  313 (0x139) finit_module sys_finit_module


  AUTHOR: Lothar Rubusch, L.Rubusch@gmx.ch


  RESOURCES:
  * https://nullprogram.com/blog/2018/06/23/ by Christopher Wellons
  * https://www.aldeid.com/wiki/Syscall
*/

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

#include <syscall.h>

#if __x86_64__
#include <sys/reg.h>
#else
#include <asm/ptrace-abi.h> /* constants, e.g. ORIG_EAX, etc. */
#endif

#include <errno.h>

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
  pid_t child;

#ifndef __x86_64__
  fprintf(stderror, "ABORTING! programmed for x86_64 only!!!\n");
  exit(EXIT_FAILURE);
#endif

  if (0 > (child = fork())) {
    perror("fork() failed");

  } else if (0 == child) {
    /* child: tracee */
    ptrace(PTRACE_TRACEME, 0, NULL, NULL);
    execl("/bin/pwd", "pwd", NULL);

  } else {
    /* parent: tracer */
    waitpid(child, 0, 0);
    ptrace(PTRACE_SETOPTIONS, child, 0, PTRACE_O_EXITKILL);

    while (1) {
      // restart the stopped tracee
      if (0 > (ptrace(PTRACE_SYSCALL, child, 0, 0))) { // PTRACE_SYSEMU ends in infinite loop here
        perror("ptrace: PTRACE_SYSCALL failed");
        exit(EXIT_FAILURE);
      }

      if (0 > waitpid(child, 0, 0)) {
        perror("waitpid: failed");
        exit(EXIT_FAILURE);
      }

      struct user_regs_struct regs;
      if (0 > ptrace(PTRACE_GETREGS, child, 0, &regs)) {
        perror("ptrace: PTRACE_GETREGS failed");
        exit(EXIT_FAILURE);
      }
/*
      fprintf(stderr
              , "syscall: '%lx(%016lx [rdi], %016lx [rsi], %016lx [rdx], %016lx [r10], %016lx [r8], %016lx [r9])'\n"
              , (long)regs.orig_rax
              , (long)regs.rdi
              , (long)regs.rsi
              , (long)regs.rdx
              , (long)regs.r10
              , (long)regs.r8
              , (long)regs.r9);
// */
      if (0 > ptrace(PTRACE_SYSCALL, child, 0, 0)) {
        perror("ptrace: process finished");
        exit(EXIT_FAILURE);
      }

      if (0 > waitpid(child, 0, 0)) {
        perror("waitpid: process finished");
        exit(EXIT_FAILURE);
      }


      switch(regs.orig_rax) {
#ifdef SYS_munmap
      case SYS_munmap:
        fprintf(stderr, "SYSCALL: SYS_munmap, '0x%04lx'\n", (long)regs.orig_rax);
        break;
#endif
#ifdef SYS_arch_prctl
      case SYS_arch_prctl:
        fprintf(stderr, "SYSCALL: SYS_arch_prctl, '0x%04lx'\n", (long)regs.orig_rax);
        break;
#endif
#ifdef SYS_getcwd
      case SYS_getcwd:
        fprintf(stderr, "SYSCALL: SYS_getcwd, '0x%04lx'\n", (long)regs.orig_rax);
        break;
#endif
#ifdef SYS_mprotect
      case SYS_mprotect:
        fprintf(stderr, "SYSCALL: SYS_mprotect, '0x%04lx'\n", (long)regs.orig_rax);
        break;
#endif
#ifdef SYS_mmap
      case SYS_mmap:
        fprintf(stderr, "SYSCALL: SYS_mmap, '0x%04lx'\n", (long)regs.orig_rax);
        break;
#endif
#ifdef SYS_close
      case SYS_close:
        fprintf(stderr, "SYSCALL: SYS_close, '0x%04lx'\n", (long)regs.orig_rax);
        break;
#endif
#ifdef SYS_brk // change the location of the program break, which defines the end of the process's data segment
      case SYS_brk:
        fprintf(stderr, "SYSCALL: SYS_brk, '0x%04lx'\n", (long)regs.orig_rax);
        break;
#endif
#ifdef SYS_openat
      case SYS_openat:
        fprintf(stderr, "SYSCALL: SYS_openat, '0x%04lx'\n", (long)regs.orig_rax);
        break;
#endif
#ifdef SYS_access
      case SYS_access:
        fprintf(stderr, "SYSCALL: SYS_access, '0x%04lx'\n", (long)regs.orig_rax);
        break;
#endif
#ifdef SYS_fstat
      case SYS_fstat:
        fprintf(stderr, "SYSCALL: SYS_fstat, '0x%04lx'\n", (long)regs.orig_rax);
        break;
#endif
#ifdef SYS_read
      case SYS_read:
        fprintf(stderr, "SYSCALL: SYS_read, '0x%04lx'\n", (long)regs.orig_rax);
        break;
#endif
#ifdef SYS_write
      case SYS_write:
        fprintf(stderr, "SYSCALL: SYS_write, '0x%04lx'\n", (long)regs.orig_rax);
        break;
#endif
#ifdef SYS_open
      case SYS_open:
        fprintf(stderr, "SYSCALL: SYS_open, '0x%04lx'\n", (long)regs.orig_rax);
        break;
#endif
#ifdef SYS_exit_group
      case SYS_exit_group:
        fprintf(stderr, "SYSCALL: SYS_exit_group, '0x%04lx'\n", (long)regs.orig_rax);
        break;
#endif

      default:
        fprintf(stderr, "SYSCALL: uncaught, '0x%04lx' XXXXXXX\n", (long)regs.orig_rax);
      }

    }
  }

  exit(EXIT_SUCCESS);
}
