## Makefile
##
## 

SOURCES = array.c
OBJECTS = ${SOURCES:.c=.o}
.PREFIXES = .c .o
CC = gcc

## leave the pedantic in the file, it's better for learning - more warnings, more fun! ;-)
CFLAGS = -g -Wall -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

array.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: array.exe

.PHONY: clean

clean:
	${RM} ${OBJECTS} array.exe *~ core

