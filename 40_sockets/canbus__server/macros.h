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
	fprintf(stderr, "%s\n", name);
#endif
}

static inline void dbg_end(const char *name)
{
#if defined DEBUG
	fprintf(stderr, "%s() ended\n", name);
#endif
}

static inline void dbg_frame(struct can_frame *frame)
{
#ifdef DEBUG
	char str[32];
	memset(str, '\0', sizeof(str));

	sprintf(str, "%03x#%02x", frame->can_id, frame->data[0]);
	for (int idx = 1; idx < frame->can_dlc; idx++) {
		sprintf(str, ".%02x", frame->data[idx]);
	}
	fprintf(stderr, "%s\n", str);
#endif
}


#endif
