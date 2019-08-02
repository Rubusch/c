# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = signal_kill.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

signal_kill: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: signal_kill

clean: 
	${RM} $(OBJECTS) signal_kill *~ core


