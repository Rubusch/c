/*
 */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>

typedef void (*sighandler_t)(int);

static bool running = false;
static int ret;

static sighandler_t connect_signal(int signum,
			      sighandler_t signalhandler)
{
	struct sigaction sa_new, sa_old;

	sa_new.sa_handler = signalhandler; /* one argument only */
	sigemptyset(&sa_new.sa_mask);
	sa_new.sa_flags = SA_RESTART;

	ret = sigaction(signum, &sa_new, &sa_old);
	if (0 > ret) {
		perror("sigaction failed");
		return SIG_ERR;
	}

	return sa_old.sa_handler;
}

static sighandler_t signal_add(int signum, sighandler_t signalhandler)
{
	struct sigaction sa;

/* If the argument act is not a null pointer, it points to a structure
 * specifying the action to be associated with the specified
 * signal. If the argument oact is not a null pointer, the action
 * previously associated with the signal is stored in the location
 * pointed to by the argument oact. If the argument act is a null
 * pointer, signal handling is unchanged; thus, the call can be used
 * to enquire about the current handling of a given signal. */
	ret = sigaction(signum, NULL, &sa); // set old sig handler
	if (0 > ret) {
		perror("sigaction failed");
		return SIG_ERR;
	}

	sa.sa_handler = signalhandler;
	sigaddset(&sa.sa_mask, signum); /* add to sa_mask */
	sa.sa_flags = SA_RESTART;

	ret = sigaction(signum, &sa, NULL); // set new sig handler
	if (0 > ret) {
		perror("sigaction failed");
		return SIG_ERR;
	}

	return sa.sa_handler;
}

static sighandler_t signal_del(int signum) {
	struct sigaction sa;

	ret = sigaction(signum, NULL, &sa);
	if (0 > ret) {
		perror("sigaction failed");
		return SIG_ERR;
	}

	sa.sa_handler = SIG_DFL;
	sigdelset(&sa.sa_mask, signum); /* remove from sa_mask */
	sa.sa_flags = SA_RESTART;

	ret = sigaction(signum, &sa, NULL);
	if (0 > ret) {
		perror("sigaction failed");
		return SIG_ERR;
	}

	return sa.sa_handler;
}

static void check_signal(int signum)
{
	struct sigaction sa;

	ret = sigaction(signum, NULL, &sa);
	if (0 > ret) {
		perror("sigaction failed");
		return;
	}

	printf("Action at signal %d: ", signum);

	if (sa.sa_handler == SIG_DFL) {
		fprintf(stderr, "%s() - sa_handler: default action (SIG_DFL)\n", __func__);
	} else if (sa.sa_handler == SIG_IGN) {
		fprintf(stderr, "%s() - sa_handler: ignore (SIG_IGN)\n", __func__);
	} else {
		fprintf(stderr, "%s() - sa_handler: call customized function to handle\n", __func__);
	}
}

static void control(int signum)
{
	int nr = -1;

	ret = 0;
	if (signum == SIGINT) {
		printf("SIGINT issued\n");
		printf("-1- remove SIGINT from mask\n");
		printf("-2- ignore signal\n");
		printf("-3- SIGALRM in 5 secs, then terminate\n");
		printf("-4- quit\n");
		printf("choose : ");
		scanf("%d", &nr);

		switch (nr) {
		case 1: /* remove */
			signal_del(SIGINT);
			printf("SIGINT removed\n");
			break;

		case 2: /* ignore */
			printf("CTRL-c to enter menu\n");
			return;

		case 3: /* sigalrm */
			alarm(5);
			break;

		case 4: /* quit */
			raise(SIGKILL);
			break;

		default:
			fprintf(stderr, "wrong input, aborting\n");
			running = false;
			ret = -1;
			break;
		}

	} else if (signum == SIGALRM) {
		printf("SIGALRM issued\n");
		raise(SIGKILL);
	}
}


int main (void)
{
	connect_signal(SIGINT, control); /* display menu at CTRL+c */

	signal_add(SIGALRM, control);
	signal_add(SIGABRT, SIG_IGN); /* ignore */

//*
	setvbuf(stdout, NULL, _IONBF, 0); /* show the 'Action at signal 2' right away (no buffering of stdout) */
	check_signal(SIGINT);
	check_signal(SIGALRM);
	check_signal(SIGABRT);
	check_signal(SIGFPE);
	setvbuf(stdout, NULL, _IOFBF, 0); /* show the 'Action at signal 2' right away (no buffering of stdout) */
	fprintf(stdout, "CTRL-c to enter menu\n");
/*/
	check_signal(SIGINT);
	check_signal(SIGALRM);
	check_signal(SIGABRT);
	check_signal(SIGFPE);
	fprintf(stderr, "CTRL-c to enter menu\n");
// */
	running = true;
	while (running)
		;

	if (0 != ret)
		exit(EXIT_FAILURE);
	exit(EXIT_SUCCESS);
}
