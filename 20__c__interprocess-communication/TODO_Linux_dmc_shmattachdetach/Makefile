# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = shm_attachdetach.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

shm_attachdetach: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm -D _SVID_SOURCE=1 -D _XOPEN_SOURCE=600

all: shm_attachdetach

clean: 
	${RM} $(OBJECTS) shm_attachdetach *~ core


