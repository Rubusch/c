# Makefile
#
#

SOURCE = stdout2file.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -O -g -Wall -std=c99
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

stdout2file: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: stdout2file

clean:
	${RM} stdout2file.exe *~ *.o core
