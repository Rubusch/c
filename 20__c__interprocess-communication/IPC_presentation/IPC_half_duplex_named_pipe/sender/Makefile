# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = sender.c
HEADERS = ../halfduplex.h
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

sender: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: sender

clean: 
	${RM} $(OBJECTS) sender *~ core


