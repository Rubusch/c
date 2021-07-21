# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = main.c selectionsort.c
HEADERS = main.h selectionsort.h
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

selectionsort.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: selectionsort.exe

.PHONY: clean

clean: 
	${RM} $(OBJECTS) selectionsort.exe  selectionsort.exe.stackdump *~ core


