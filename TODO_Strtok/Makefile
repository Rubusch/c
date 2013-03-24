# Makefile
#
#

SOURCE = Strtok.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -O -g
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

Strtok: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: Strtok

clean: 
	${RM} Strtok.exe *~ *.o core


