# Makefile (simple)
#
# use -O, -O1, -O2 for optimization


SOURCE = sigmask.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

sigmask.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: sigmask.exe

clean:
	${RM} $(OBJECTS) sigmask.exe *~ core


