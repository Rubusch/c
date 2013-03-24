# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = vfork.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

vfork: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: vfork

clean: 
	${RM} $(OBJECTS) vfork *~ core


