# Makefile
#
#

SOURCE = filesize.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -O -g -Wall -std=c99
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

filesize: ${OBJECTS} 
	${CC} -o $@ ${OBJECTS} -lm

all: filesize

clean:
	-${RM} filesize.exe *~ *.o core
