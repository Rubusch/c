# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = signaltalk.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

signaltalk: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: signaltalk

clean: 
	${RM} $(OBJECTS) signaltalk *~ core


