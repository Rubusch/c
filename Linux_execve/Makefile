# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = execve.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

execve.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: execve.exe

clean: 
	${RM} $(OBJECTS) execve.exe *~ core


