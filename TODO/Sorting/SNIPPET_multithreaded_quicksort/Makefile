# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = quicksort.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

quicksort: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm -pthread

all: quicksort

clean: 
	${RM} $(OBJECTS) quicksort *~ core


