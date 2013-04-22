## Makefile (simple)
##
## @author: Lothar Rubusch
## @email: L.Rubusch@gmx.ch
## @license: GPLv3
##
## use -O, -O1, -O2 for optimization
SOURCE = argumentlist.c
OBJECTS = ${SOURCE:%.c=%.o}
CC = gcc
CFLAGS = -g -Wall -std=c99 -pedantic
RM = rm -f

%.o: %.c
	${CC} -c ${CFLAGS} $<

argumentlist.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: argumentlist.exe

clean:
	${RM} $(OBJECTS) argumentlist.exe *~

.PHONY: all clean
