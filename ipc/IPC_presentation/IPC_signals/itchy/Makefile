# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = itchy.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

itchy: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: itchy

clean: 
	${RM} $(OBJECTS) itchy *~ core


