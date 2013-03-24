# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 
# FIXME

SOURCE = semctl.c console_input.c
HEADERS = console_input.h
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

semctl: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: semctl

clean: 
	${RM} $(OBJECTS) semctl *~ core


