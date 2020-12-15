//setup_signal.c

#include <signal.h> // signal(), sigaction()


void* setup_signal( int signo, void* func )
{
        struct sigaction act, oact;

        act.sa_handler = func;
        sigemptyset(&act.sa_mask);
        act.sa_flags = 0;

        // handle specific cases
        if (signo == SIGALRM) {
#ifdef SA_INTERRUPT
// SunOS 4.x
                act.sa_flags |= SA_INTERRUPT;
#endif
        } else {
#ifdef  SA_RESTART
// SVR4, 44BSD
                act.sa_flags |= SA_RESTART;
#endif
        }

        // sigaction
        if (0 > sigaction(signo, &act, &oact)) {
                return SIG_ERR;
        }

        return oact.sa_handler;
}
