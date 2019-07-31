# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = struct.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

Struct: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: Struct

clean: 
	${RM} $(OBJECTS) Struct.exe *~ core
