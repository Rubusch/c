# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = execl.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

execl.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: execl.exe

clean: 
	${RM} $(OBJECTS) execl.exe *~ core


