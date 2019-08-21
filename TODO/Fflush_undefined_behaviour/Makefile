# Makefile (simple)
#
# use -O, -O1, -O2 for optimization


SOURCE = fflush.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -Werror -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} ${CFLAGS} -c $<

fflush.exe: ${OBJECTS}
	${CC} ${CFLAGS} -o $@ ${OBJECTS} -lm

all: fflush.exe

clean:
	${RM} $(OBJECTS) fflush.exe *~ core
