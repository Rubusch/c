# Makefile (simple)
#
# use -O, -O1, -O2 for optimization


SOURCE = func_ptr.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -Werror -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

funcptr.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: funcptr.exe

clean:
	${RM} $(OBJECTS) funcptr.exe *~ core


