# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = gidpid.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

gidpid: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: gidpid

clean: 
	${RM} $(OBJECTS) gidpid *~ core


