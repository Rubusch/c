# Makefile
#
# .h and .c files

SOURCE = main.c directory/folder.c
HEADERS = directory/folder.h
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -std=gnu99
RM = rm

.c.o:
#	${CC} -c ${CFLAGS} $<
	${CC} -o $@ -c ${CFLAGS} $< 	

prgname: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all:	prgname

clean:
	${RM} ${OBJECTS} prgname *~


