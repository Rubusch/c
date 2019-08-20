# Makefile (simple)
#
# use -O, -O1, -O2 for optimization


SOURCE = input.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -Werror -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} ${CFLAGS} -c $<

input.exe: ${OBJECTS}
	${CC} ${CFLAGS} -o $@ ${OBJECTS} -lm

all: input.exe

clean:
	${RM} $(OBJECTS) input.exe *~ core


