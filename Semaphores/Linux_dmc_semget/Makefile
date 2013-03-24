# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = semget.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

semget: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm -D _SVID_SOURCE -D _XOPEN_SOURCE

all: semget

clean: 
	${RM} $(OBJECTS) semget *~ core


