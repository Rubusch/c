# Makefile
#
#

SOURCE = fseek.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -Werror -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} ${CFLAGS} -c $<

fseek.exe: ${OBJECTS}
	cp -f template.txt text.txt
	${CC} ${CFLAGS} -o $@ ${OBJECTS} -lm

all: fseek.exe


clean:
	-${RM} fseek.exe *.o *~ core error.log
