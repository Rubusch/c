# Makefile
#
#

SOURCE = Sizeof.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -O -g -Wall -std=c99
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

Sizeof: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: Sizeof

clean: 
	${RM} Sizeof.exe *~ *.o core


