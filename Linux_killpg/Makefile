# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = killpg.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

killpg: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: killpg

clean: 
	${RM} $(OBJECTS) killpg *~ core


