# Makefile
#
#

SOURCE = preprocessor.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall
RM = rm -f

.c.o: 
	${CC} -c ${SOURCE} $<

preprocessor: ${OBJECTS}
	$(CC) -o $@ ${OBJECTS} -lm

all: preprocessor

clean:
	${RM} preprocessor.exe *.o *~ core

