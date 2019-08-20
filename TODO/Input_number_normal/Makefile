# Makefile (simple)
#
# @author: Lothar Rubusch
# @email: L.Rubusch@gmx.ch
# @license: GPLv3
# @2013-May-01
#
# use -O, -O1, -O2 for optimization


SOURCE = input_number_normal.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -Werror -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} ${CFLAGS} -c $<

input.exe: ${OBJECTS}
	${CC} ${CFLAGS} -o $@ ${OBJECTS} -lm

all: input.exe

clean:
	${RM} $(OBJECTS) input.exe *~ core


