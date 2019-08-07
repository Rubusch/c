# Makefile
#
#

SOURCE = filesize.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -O -g -Wall -Werror -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

file.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: file.exe

clean:
	-${RM} file.exe *~ *.o core
