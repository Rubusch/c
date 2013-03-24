# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = execv.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

execv.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: execv.exe

clean: 
	${RM} $(OBJECTS) execv.exe *~ core


