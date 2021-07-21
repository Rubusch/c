# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = main.c shakersort.c
HEADERS = main.h shakersort.h
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

shakersort.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: shakersort.exe

.PHONY: clean

clean: 
	${RM} $(OBJECTS) shakersort.exe  shakersort.exe.stackdump *~ core


