# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = shmctl.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

shmctl: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm -D _SVID_SOURCE=1 -D _XOPEN_SOURCE=600

all: shmctl

clean: 
	${RM} $(OBJECTS) shmctl *~ core


