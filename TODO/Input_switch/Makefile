# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = input_switch.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<


all: input.exe

input.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm


clean:
	${RM} $(OBJECTS) input.exe *~ core
