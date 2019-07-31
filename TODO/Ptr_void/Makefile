# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = void_ptr.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

Void_ptr: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: Void_ptr

clean: 
	${RM} $(OBJECTS) Void_ptr.exe *~ core


