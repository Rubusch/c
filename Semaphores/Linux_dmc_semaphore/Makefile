# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = semaphore.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

semaphore: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm -D _SVID_SOURCE -D _XOPEN_SOURCE

all: semaphore

clean: 
	${RM} $(OBJECTS) semaphore *~ core


