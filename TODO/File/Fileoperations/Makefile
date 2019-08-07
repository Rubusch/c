# Makefile
#
#

SOURCE = main.c fo.c
HEADERS = fo.h
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

file.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: file.exe

clean:
	${RM} file.exe.exe ${OBJECTS} *~ *.txt core

