# Makefile
#
#

SOURCE = file_appending.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -O -g -Wall -std=c99
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

file_appending.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: file_appending.exe

.PHONY: clean

clean: 
	${RM} file_appending.exe *~ *.o core


