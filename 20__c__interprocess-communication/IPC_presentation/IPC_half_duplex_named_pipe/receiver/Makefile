# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = receiver.c
HEADERS = ../half_duplex.h
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

receiver: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: receiver

clean: 
	${RM} $(OBJECTS) receiver *~ core


