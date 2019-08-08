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
	${CC} -c ${CFLAGS} $<

Hello.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

clean:
	${RM} ${OBJECTS} Hello.exe *~


