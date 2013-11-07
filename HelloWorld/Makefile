#	Makefile Dummy
#
#

SOURCE = test.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -pedantic
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<
	
Test.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

clean:
	${RM} ${OBJECTS} Test.exe *~


