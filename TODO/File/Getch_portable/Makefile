# Makefile
#
#

SOURCE = getch.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -Werror -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} ${CFLAGS} -c $<

opengetch.exe: ${OBJECTS}
	${CC} ${CFLAGS} -o $@ ${OBJECTS} -lm

all: opengetch.exe

clean:
	-${RM} opengetch.exe *.o *~ core
