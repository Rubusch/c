# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = main.c shellsort.c
HEADERS = main.h shellsort.h
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

shellsort.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: shellsort.exe

.PHONY: clean

clean: 
	${RM} $(OBJECTS) shellsort.exe  shellsort.exe.stackdump *~ core


