  NOTES

  #include <sys/wait.h>
  pid_t waitpid(pid_t pid, int* status, int options);

  As wait(), but wait's on a particular child process (pid) which doesn't need
  to be the first one.

  PARAMS of waitpid:
  pid - pid of the child process
  status - ???
  options - 3 possibilities:
      WCONTINUED func shall report the status of any continued child process
                 specified by pid whose status has not been reported since it
                 continued from a job control stop

      WNOHANG    func shall not suspend execution of the calling thread of
                 status is not immediately available for the one of the child
                 process pid

      WUNTRACED  status of any child processes specified by pid that are
                 stopped shall be reported to the requested process


  MACROS

  WIFEXITED()    evaluates to a non-zero value if "status" was returned for a
                 child process that terminated normally
                 => true if child exited due to exit() or _exit()

  WEXITSTATUS()  if the value is non-zero, this macro evaluates to the low-order
                 8 bits of "status" that the child process passed to _exit() or
                 exit(), or the value the child pid returned from main()
                 => contains the exit code in case of exit() or _exit()

  WIFSIGNALED()  evaluates to a non-zero value of "status"was returned for a
                 child process that terminated due to the receipt of a signal
                 that was not caught
                 => true, if child exited due to a signal

  WTERMSIG()     if WIFSIGNALED is non-zero, this macro evaluates to the number
                 of the signal that caused the termination of the child pid
                 => contains the signal number in case of a signal

  WIFSTOPPED()   evaluates the value of WFISTOPPED is non-zero, this macro
                 evaluates to the number of the signal that caused the child
		 process to stop
                 => contains if child stopped (signal)

  WSTOPSIG()     if the value of WIFSTOPPED is non-zero, this macro evaluates
                 to the number of the signal that caused the child process to
		 stop



  ERRORs

  ECHILD         child does not exist or pid is not a child
  EINVAL         invalid "option"
  EINTR          func was interrupted by a signal


TODO: ptrace()


PTRACE_TRACEME
    Indicate that this process is to be traced by its parent. A process probably shouldn't make this request if its parent isn't expecting to trace it. (pid, addr, and data are ignored.)

The PTRACE_TRACEME request is used only by the tracee; the remaining requests are used only by the tracer. In the following requests, pid specifies the thread ID of the tracee to be acted on. For requests other than PTRACE_ATTACH, PTRACE_SEIZE, PTRACE_INTERRUPT and PTRACE_KILL, the tracee must be stopped.


PTRACE_PEEKTEXT, PTRACE_PEEKDATA
    Read a word at the address addr in the tracee's memory, returning the word as the result of the ptrace() call. Linux does not have separate text and data address spaces, so these two requests are currently equivalent. (data is ignored.) 

PTRACE_PEEKUSER
    Read a word at offset addr in the tracee's USER area, which holds the registers and other information about the process (see <sys/user.h>). The word is returned as the result of the ptrace() call. Typically, the offset must be word-aligned, though this might vary by architecture. See NOTES. (data is ignored.) 

PTRACE_POKETEXT, PTRACE_POKEDATA
    Copy the word data to the address addr in the tracee's memory. As for PTRACE_PEEKTEXT and PTRACE_PEEKDATA, these two requests are currently equivalent. 

PTRACE_POKEUSER
    Copy the word data to offset addr in the tracee's USER area. As for PTRACE_PEEKUSER, the offset must typically be word-aligned. In order to maintain the integrity of the kernel, some modifications to the USER area are disallowed. 

PTRACE_GETREGS, PTRACE_GETFPREGS
    Copy the tracee's general-purpose or floating-point registers, respectively, to the address data in the tracer. See <sys/user.h> for information on the format of this data. (addr is ignored.) Note that SPARC systems have the meaning of data and addr reversed; that is, data is ignored and the registers are copied to the address addr. PTRACE_GETREGS and PTRACE_GETFPREGS are not present on all architectures. 

PTRACE_GETREGSET (since Linux 2.6.34)
    Read the tracee's registers. addr specifies, in an architecture-dependent way, the type of registers to be read. NT_PRSTATUS (with numerical value 1) usually results in reading of general-purpose registers. If the CPU has, for example, floating-point and/or vector registers, they can be retrieved by setting addr to the corresponding NT_foo constant. data points to a struct iovec, which describes the destination buffer's location and length. On return, the kernel modifies iov.len to indicate the actual number of bytes returned. 

PTRACE_GETSIGINFO (since Linux 2.3.99-pre6)
    Retrieve information about the signal that caused the stop. Copy a siginfo_t structure (see sigaction(2)) from the tracee to the address data in the tracer. (addr is ignored.) 

PTRACE_SETREGS, PTRACE_SETFPREGS
    Modify the tracee's general-purpose or floating-point registers, respectively, from the address data in the tracer. As for PTRACE_POKEUSER, some general-purpose register modifications may be disallowed. (addr is ignored.) Note that SPARC systems have the meaning of data and addr reversed; that is, data is ignored and the registers are copied from the address addr. PTRACE_SETREGS and PTRACE_SETFPREGS are not present on all architectures. 

PTRACE_SETREGSET (since Linux 2.6.34)
    Modify the tracee's registers. The meaning of addr and data is analogous to PTRACE_GETREGSET. 

PTRACE_SETSIGINFO (since Linux 2.3.99-pre6)
    Set signal information: copy a siginfo_t structure from the address data in the tracer to the tracee. This will affect only signals that would normally be delivered to the tracee and were caught by the tracer. It may be difficult to tell these normal signals from synthetic signals generated by ptrace() itself. (addr is ignored.) 

PTRACE_SETOPTIONS (since Linux 2.4.6; see BUGS for caveats)
    Set ptrace options from data. (addr is ignored.) data is interpreted as a bit mask of options, which are specified by the following flags: 

PTRACE_O_EXITKILL (since Linux 3.8)
    If a tracer sets this flag, a SIGKILL signal will be sent to every tracee if the tracer exits. This option is useful for ptrace jailers that want to ensure that tracees can never escape the tracer's control. 

PTRACE_O_TRACECLONE (since Linux 2.5.46)
    Stop the tracee at the next clone(2) and automatically start tracing the newly cloned process, which will start with a SIGSTOP. A waitpid(2)

    by the tracer will return a status value such that

    status>>8 == (SIGTRAP | (PTRACE_EVENT_CLONE<<8))

    The PID of the new process can be retrieved with PTRACE_GETEVENTMSG. 
This option may not catch
    clone(2) calls in all cases. If the tracee calls clone(2) with the CLONE_VFORK flag, PTRACE_EVENT_VFORK will be delivered instead if PTRACE_O_TRACEVFORK is set; otherwise if the tracee calls clone(2) with the exit signal set to SIGCHLD, PTRACE_EVENT_FORK will be delivered if PTRACE_O_TRACEFORK is set. 

PTRACE_O_TRACEEXEC (since Linux 2.5.46)
    Stop the tracee at the next execve(2). A waitpid(2) by the tracer will return a status

    value such that

    status>>8 == (SIGTRAP | (PTRACE_EVENT_EXEC<<8))

    If the execing thread is not a thread group leader, the thread ID is reset to thread group leader's ID before this stop. Since Linux 3.0, the former thread ID can be retrieved with PTRACE_GETEVENTMSG. 

PTRACE_O_TRACEEXIT (since Linux 2.5.60)
    Stop the tracee at exit. A waitpid(2) by the tracer will return

    a status value such that

    status>>8 == (SIGTRAP | (PTRACE_EVENT_EXIT<<8))

    The tracee's exit status can be retrieved with PTRACE_GETEVENTMSG. 
The tracee is stopped early during process exit,
    when registers are still available, allowing the tracer to see where the exit occurred, whereas the normal exit notification is done after the process is finished exiting. Even though context is available, the tracer cannot prevent the exit from happening at this point. 

PTRACE_O_TRACEFORK (since Linux 2.5.46)
    Stop the tracee at the next fork(2) and automatically start tracing the newly forked process, which will start with a SIGSTOP. A waitpid(2) by the tracer will return

    a status value such that

    status>>8 == (SIGTRAP | (PTRACE_EVENT_FORK<<8))

    The PID of the new process can be retrieved with PTRACE_GETEVENTMSG. 

PTRACE_O_TRACESYSGOOD (since Linux 2.4.6)
    When delivering system call traps, set bit 7 in the signal number (i.e., deliver SIGTRAP|0x80). This makes it easy for the tracer to distinguish normal traps from those caused by a system call. (PTRACE_O_TRACESYSGOOD may not work on all architectures.) 

PTRACE_O_TRACEVFORK (since Linux 2.5.46)
    Stop the tracee at the next vfork(2) and automatically start tracing the newly vforked process, which will start with a SIGSTOP. A waitpid(2) by

    the tracer will return a status value such that

    status>>8 == (SIGTRAP | (PTRACE_EVENT_VFORK<<8))

    The PID of the new process can be retrieved with PTRACE_GETEVENTMSG. 

PTRACE_O_TRACEVFORKDONE (since Linux 2.5.60)
    Stop the tracee at the completion of the next vfork(2). A waitpid(2) by the tracer will return a status value such that

    status>>8 == (SIGTRAP | (PTRACE_EVENT_VFORK_DONE<<8))

    The PID of the new process can (since Linux 2.6.18) be retrieved with PTRACE_GETEVENTMSG. 

PTRACE_GETEVENTMSG (since Linux 2.5.46)
    Retrieve a message (as an unsigned long) about the ptrace event that just happened, placing it at the address data in the tracer. For PTRACE_EVENT_EXIT, this is the tracee's exit status. For PTRACE_EVENT_FORK, PTRACE_EVENT_VFORK, PTRACE_EVENT_VFORK_DONE, and PTRACE_EVENT_CLONE, this is the PID of the new process. (addr is ignored.) 

PTRACE_CONT
    Restart the stopped tracee process. If data is nonzero, it is interpreted as the number of a signal to be delivered to the tracee; otherwise, no signal is delivered. Thus, for example, the tracer can control whether a signal sent to the tracee is delivered or not. (addr is ignored.) 

PTRACE_SYSCALL, PTRACE_SINGLESTEP
    Restart the stopped tracee as for PTRACE_CONT, but arrange for the tracee to be stopped at the next entry to or exit from a system call, or after execution of a single instruction, respectively. (The tracee will also, as usual, be stopped upon receipt of a signal.) From the tracer's perspective, the tracee will appear to have been stopped by receipt of a SIGTRAP. So, for PTRACE_SYSCALL, for example, the idea is to inspect the arguments to the system call at the first stop, then do another PTRACE_SYSCALL and inspect the return value of the system call at the second stop. The data argument is treated as for PTRACE_CONT. (addr is ignored.) 

PTRACE_SYSEMU, PTRACE_SYSEMU_SINGLESTEP (since Linux 2.6.14)
    For PTRACE_SYSEMU, continue and stop on entry to the next system call, which will not be executed. For PTRACE_SYSEMU_SINGLESTEP, do the same but also singlestep if not a system call. This call is used by programs like User Mode Linux that want to emulate all the tracee's system calls. The data argument is treated as for PTRACE_CONT. The addr argument is ignored. These requests are currently supported only on x86. 

PTRACE_LISTEN (since Linux 3.4)
    Restart the stopped tracee, but prevent it from executing. The resulting state of the tracee is similar to a process which has been stopped by a SIGSTOP (or other stopping signal). See the "group-stop" subsection for additional information. PTRACE_LISTEN only works on tracees attached by PTRACE_SEIZE. 

PTRACE_KILL
    Send the tracee a SIGKILL to terminate it. (addr and data are ignored.) 
This operation is deprecated; do not use it!
    Instead, send a SIGKILL directly using kill(2) or tgkill(2). The problem with PTRACE_KILL is that it requires the tracee to be in signal-delivery-stop, otherwise it may not work (i.e., may complete successfully but won't kill the tracee). By contrast, sending a SIGKILL directly has no such limitation. 

PTRACE_INTERRUPT (since Linux 3.4)
    Stop a tracee. If the tracee is running, it will stop with PTRACE_EVENT_STOP. If the tracee is already stopped by a signal, or receives a signal in parallel with PTRACE_INTERRUPT, it may report a group-stop or a signal-delivery-stop instead of PTRACE_EVENT_STOP. PTRACE_INTERRUPT only works on tracees attached by PTRACE_SEIZE. 

PTRACE_ATTACH
    Attach to the process specified in pid, making it a tracee of the calling process. The tracee is sent a SIGSTOP, but will not necessarily have stopped by the completion of this call; use waitpid(2) to wait for the tracee to stop. See the "Attaching and detaching" subsection for additional information. (addr and data are ignored.) 

PTRACE_SEIZE (since Linux 3.4)
    Attach to the process specified in pid, making it a tracee of the calling process. Unlike PTRACE_ATTACH, PTRACE_SEIZE does not stop the process. Only a PTRACE_SEIZEd process can accept PTRACE_INTERRUPT and PTRACE_LISTEN commands. addr must be zero. data contains a bit mask of ptrace options to activate immediately. 

PTRACE_DETACH
    Restart the stopped tracee as for PTRACE_CONT, but first detach from it. Under Linux, a tracee can be detached in this way regardless of which method was used to initiate tracing. (addr is ignored.)
