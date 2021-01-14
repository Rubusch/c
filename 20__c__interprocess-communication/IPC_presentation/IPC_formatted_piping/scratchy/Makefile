# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = scratchy.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

scratchy: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: scratchy

clean: 
	${RM} $(OBJECTS) scratchy *~ core


