# Makefile
#
#

SOURCE = fsetposition.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -Werror -std=c99
RM = rm -f

.c.o:
	${CC} ${CFLAGS} -c $<

fsetposition.exe: ${OBJECTS}
	cp -f template.txt text.txt
	${CC} ${CFLAGS} -o $@ ${OBJECTS} -lm

all: fsetposition.exe

clean:
	${RM} fsetposition.exe *~ *.o core error.log text.txt
