# Makefile
#
#

SOURCE = pass_by_value.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

pass_by_value.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: pass_by_value.exe

.PHONY: clean

clean: 
	${RM} pass_by_value.exe *~ *.o core


