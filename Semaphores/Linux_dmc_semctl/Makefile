# Makefile (simple)
#
# use -O, -O1, -O2 for optimization

SOURCE = semctl.c console_input.c
HEADERS = console_input.h
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<


all: semctl

semctl: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm


clean:
	${RM} $(OBJECTS) semctl *~ core
