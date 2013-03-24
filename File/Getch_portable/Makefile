# Makefile
#
#

SOURCE = getch.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -O -g -Wall -std=c99
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

opengetch: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: opengetch

clean:
	-${RM} opengetch *.o *~ core
