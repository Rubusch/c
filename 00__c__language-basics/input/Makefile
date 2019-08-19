## Makefile
##
## @author: Lothar Rubusch
## @email: L.Rubusch@gmx.ch
## @license: GPLv3
##
## .h and .c files

SOURCE = main.c console_input.c
HEADERS = console_input.h
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -Werror -std=gnu99
RM = rm -f

.c.o:
	${CC} ${CFLAGS} -c $<

input.exe: ${OBJECTS}
	${CC} ${CFLAGS} -o $@ ${OBJECTS} -lm

all:	input.exe

clean:
	${RM} ${OBJECTS} input.exe *~


