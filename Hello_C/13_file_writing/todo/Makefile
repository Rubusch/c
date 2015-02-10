# Makefile
#
#

SOURCE = file_writing.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -O -g -Wall -std=c99
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

file_writing.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: file_writing.exe

.PHONY: clean

clean: 
	${RM} file_writing.exe *~ *.o core


