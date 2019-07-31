## Makefile
##
## 

SOURCES = length.c
OBJECTS = ${SOURCES:.c=.o}
.PREFIXES = .c .o
CC = gcc

## leave the pedantic in the file, it's better for learning - more warnings, more fun! ;-)
CFLAGS = -g -Wall -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

length.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: length.exe

.PHONY: clean

clean:
	${RM} ${OBJECTS} length.exe *~ core

