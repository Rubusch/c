# Makefile
#
#

SOURCE = fseek.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -O -g -Wall -std=c99
RM = rm -f

.c.o: 
	${CC} -c ${CFLAGS} $<

fseek: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: fseek

clean:
	-${RM} fseek.exe *.o *~ core
