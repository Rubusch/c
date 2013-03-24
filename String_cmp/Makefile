# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = String_cmp.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

String_cmp: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: String_cmp

clean: 
	${RM} $(OBJECTS) String_cmp.exe *~ core


