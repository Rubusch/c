# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = signal.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

signal: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: signal

clean: 
	${RM} $(OBJECTS) signal *~ core


