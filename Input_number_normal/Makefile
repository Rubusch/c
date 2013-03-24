# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = input_number_normal.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

input: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: input

clean: 
	${RM} $(OBJECTS) input *~ core


