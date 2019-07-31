# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 
# FIXME

SOURCE = semop.c lothar/console_input.c
HEADERS = lothar/console_input.h
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall
RM = rm -f

.c.o:
	${CC} -o $@ -c ${CFLAGS} $<

semop: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: semop

clean: 
	${RM} $(OBJECTS) semop *~ core


