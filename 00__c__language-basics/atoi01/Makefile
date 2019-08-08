## Makefile
##
## @author: Lothar Rubusch
## @email: L.Rubusch@gmx.ch
## @license: GPLv3
##

SOURCE = atoi01.c
OBJECTS = ${SOURCE:.c =.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -O2 -g -Wall -Werror -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} -o ${CFLAGS} $<

atoi01.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: atoi01.exe

clean:
	-${RM} atoi01.exe *.o core


