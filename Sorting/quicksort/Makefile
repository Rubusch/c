# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = main.c quicksort.c
HEADERS = main.h quicksort.h
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall
# to turn off the "asserts"
#CFLAGS = -g -Wall -DNDEBUG
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

quicksort.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: quicksort.exe

.PHONY: clean

clean: 
	${RM} $(OBJECTS) quicksort.exe  quicksort.exe.stackdump *~ core


