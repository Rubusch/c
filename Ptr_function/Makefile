# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = func_ptr.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

Func_ptr: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: Func_ptr

clean: 
	${RM} $(OBJECTS) Func_ptr.exe *~ core


