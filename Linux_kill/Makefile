# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = kill.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

kill: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: kill

clean: 
	${RM} $(OBJECTS) kill *~ core


