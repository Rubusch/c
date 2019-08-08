# Makefile (simple)
#
# use -O, -O1, -O2 for optimization


SOURCE = void_ptr.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -Werror -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} ${CFLAGS} -c $<

voidptr.exe: ${OBJECTS}
	${CC} ${CFLAGS} -o $@ ${OBJECTS} -lm

all: voidptr.exe

clean:
	${RM} $(OBJECTS) voidptr.exe *~ core


