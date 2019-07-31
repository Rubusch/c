## Makefile
##
##

SOURCE = overwrite.c
OBJECTS = ${SOURCE:.c=.o}
.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

overwrite: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: overwrite

clean:
	${RM} ${OBJECTS} overwrite *~ core


