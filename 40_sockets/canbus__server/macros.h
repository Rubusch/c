#ifndef MACROS_H
#define MACROS_H

#include <linux/can.h>

/* debug print */
#define DEBUG 1

/* mock for unittest */
//#define TESTING 1


static inline void dbg(const char *name)
{
#ifdef DEBUG
	fprintf(stderr, "%s()\n", name);
#endif
}

static inline void dbg_end(const char *name)
{
#if defined DEBUG
	fprintf(stderr, "%s() ended\n", name);
#endif
}

static inline void dbg_frame(const char *func, struct can_frame *frame)
{
#ifdef DEBUG
	char str[34];
	memset(str, '\0', sizeof(str));

	if (NULL == frame) {
		strcpy(str, "0x0");
	} else {
		// id, seqn
		sprintf(str, "%03x#%02x", frame->can_id, frame->data[0]);

		// payload
		for (int idx = 1; idx < frame->can_dlc; idx++) {
			char tmp[4];
			sprintf(tmp, ".%02x", frame->data[idx]);
			strcat(str, tmp);
		}
	}
	fprintf(stderr, "%s(): %s\n", func, str);
#endif
}


#endif
