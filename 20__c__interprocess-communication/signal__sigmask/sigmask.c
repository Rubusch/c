// sigmask.c
/*
  @author: Lothar Rubusch
  @email: L.Rubusch@gmx.ch
  @license: GPLv3
  @2013-october-22



  The program asks if the user really wants to end the prgs, counts the CTRL+C's
  and ends on a SIGKILL.


  Demonstrates the use of signal masks to block further incoming signals and
  avoid race conditions.


  How to set up a signal mask

  // define a new mask set
  sigset_t mask_set;

  // first clear the set (i.e. make it contain no signal numbers)
  sigemtpyset(&mask_set);

  // lets add the TSTP and INT signals to our mask set
  sigaddset(&mask_set, SIGTSTP);
  sigaddset(&mask_set, SIGINT);

  // and just for fun, lets remove the TSTP signal from the set
  sigdelset(&mask_set, SIGTSTP);

  // finally, lets check if the INT signal is defined in our set
  if( sigismember(&mask_set, SIGINT)){
    printf("signal INT is in our set\n");
  }else{
    printf("signal INT is not in our set - how strange...\n");
  }

  // finally lets make the set contain ALL signals available on our system
  // sigfillset(&mask_set);



  Problem: race conditions are still possible, better to use diretly
  sigaction() or on older systems sigvec()
*/

#define _XOPEN_SOURCE 600 /* sigmask & CO */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

// define the CTRL+C counter, initialize it with zero
int ctrl_c_count = 0;
#define CTRL_C_THRESHOLD 5

// the signal handler for CTRL+C
void catch_int(int sig)
{
	sigset_t mask_set; // used to set a signal masking set
	sigset_t old_set; // used to store the old mask set

	// re-set the signal handler again to catch_int, for next time -> Linux
	signal(SIGINT, catch_int);

	// mask any further signals while we're inside the handler
	// use sigaction() or the older sigvec() to have a saver program than this!
	sigfillset(&mask_set);
	sigprocmask(SIG_SETMASK, &mask_set, &old_set);

	// increase count and check if threshold was reached
	++ctrl_c_count;
	if (ctrl_c_count >= CTRL_C_THRESHOLD) {
		char answer[30];

		// prompt the user to tell us if to really exit or not
		printf("\nReally Exit? [y/n]: ");
		fflush(stdout);
		fgets(answer, 30, stdin);

		if (answer[0] == 'y' || answer[0] == 'Y') {
			printf("\nExiting...\n");
			fflush(stdout);
			exit(0);

		} else {
			printf("\nContinuing\n");
			fflush(stdout);

			// reset CTRL + C counter
			ctrl_c_count = 0;
		}
	}
	/*
	  no need to restore the old signal mask - this is done automatically,
	  by the operating system, when a signal handler returns.
	//*/
}

void catch_suspend(int sig)
{
	sigset_t mask_set; // sig mask
	sigset_t old_set; // restore

	// re-set the signal handler again to catch_suspend, for next time
	signal(SIGTSTP, catch_suspend);

	// mask any further signals while we're inside the handler
	sigfillset(&mask_set);
	sigprocmask(SIG_SETMASK, &mask_set, &old_set);

	// print the current CTRL+C counter
	printf("\n\nso far, '%d' CTRL+C presses were counted\n\n",
	       ctrl_c_count);
	fflush(stdout);

	/*
	  no need to restore the old signal mask - this is done automatically,
	  by the operating system, when a signal handler returns.
	//*/
}

int main(int argc, char **argv)
{
	fprintf(stderr, "End the program by %d times pressing CTRL+^C\n",
		CTRL_C_THRESHOLD); /* stderr, unbuffered! */

	signal(SIGINT, catch_int);
	signal(SIGTSTP, catch_suspend);

	while (1)
		pause();

	// never executed
	exit(0);
}
