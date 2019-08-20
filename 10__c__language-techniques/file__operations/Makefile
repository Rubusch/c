# Makefile
#
#

SOURCE = main.c fo.c
HEADERS = fo.h
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -Werror -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} ${CFLAGS} -c $<

file.exe: ${OBJECTS}
	${CC} ${CFLAGS} -o $@ ${OBJECTS} -lm

all: file.exe

clean:
	${RM} file.exe ${OBJECTS} *~ *.txt core

