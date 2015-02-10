# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = execlp.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

execlp.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: execlp.exe

clean: 
	${RM} $(OBJECTS) execlp.exe *~ core


