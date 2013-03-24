# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = timer.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

timer: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: timer

clean: 
	${RM} $(OBJECTS) timer *~ core


