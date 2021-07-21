# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = PrgName.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

PrgName: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: PrgName

clean: 
	${RM} $(OBJECTS) PrgName.exe *~ core


