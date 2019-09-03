# Makefile (simple)
#
# use -O, -O1, -O2 for optimization


SOURCE = daemon.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -Werror -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} ${CFLAGS} -c $<

daemon.exe: ${OBJECTS}
	${CC} ${CFLAGS} -o $@ ${OBJECTS} -lm

all: daemon.exe

clean:
	${RM} $(OBJECTS) daemon.exe *~ core


