# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = popen.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

popen: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: popen

clean: 
	${RM} $(OBJECTS) popen *~ core


