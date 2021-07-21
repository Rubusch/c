# Makefile
#
#

SOURCE = file_reading.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -O -g -Wall -std=c99
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

file_reading.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: file_reading.exe

.PHONY: clean

clean: 
	${RM} file_reading.exe *~ *.o core


