#	Makefile Dummy
#
#

SOURCE = Hello.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -Werror -pedantic -std=c99
RM = rm -f

.c.o:
	${CC} ${CFLAGS} -c $<

Hello.exe: ${OBJECTS}
	${CC} ${CFLAGS} -o $@ ${OBJECTS} -lm

clean:
	${RM} ${OBJECTS} Hello.exe *~


