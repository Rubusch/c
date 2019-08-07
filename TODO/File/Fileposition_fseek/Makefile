# Makefile
#
#

SOURCE = fseek.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -O -g -Wall -Werror -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

fseek.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: fseek.exe

clean:
	-${RM} fseek.exe *.o *~ core
