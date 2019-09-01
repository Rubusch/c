# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = struct.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -Werror -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} ${CFLAGS} -c $<

struct.exe: ${OBJECTS}
	${CC} ${CFLAGS} -o $@ ${OBJECTS} -lm

all: struct.exe

clean:
	${RM} $(OBJECTS) struct.exe *~ core
