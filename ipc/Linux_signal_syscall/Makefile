# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = signal_syscall.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

signal_syscall: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: signal_syscall

clean: 
	${RM} $(OBJECTS) signal_syscall *~ core


