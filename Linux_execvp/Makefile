# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = execvp.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

execvp.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: execvp.exe

clean: 
	${RM} $(OBJECTS) execvp.exe *~ core


