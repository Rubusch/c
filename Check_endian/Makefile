# Makefile
#
#

SOURCE = endian.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -O -g -Wall -std=c99
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

checkEndian: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: checkEndian

clean: 
	${RM} checkEndian.exe *~ *.o core


