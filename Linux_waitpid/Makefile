# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = waitpid.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

waitpid: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: waitpid

clean: 
	${RM} $(OBJECTS) waitpid *~ core


