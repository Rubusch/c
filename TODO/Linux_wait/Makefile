# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = wait.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

wait.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: wait.exe

clean: 
	${RM} $(OBJECTS) wait.exe *~ core


