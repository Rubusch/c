# Makefile
# Lothar Rubusch (2013)
#
# use -O, -O1, -O2 for optimization


SOURCE = ptracer.c rabbit.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
#CFLAGS = -g -Wall -std=c99 -pedantic # doesn't work with sigaction
CFLAGS = -g -Wall
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<


all: ptracer.exe rabbit.exe

ptracer.exe: ptracer.o
	${CC} -o $@ ptracer.o -lm

rabbit.exe: rabbit.o
#	${CC} -o $@ rabbit.o -lm
	${CC} -static -o $@ rabbit.o -lm


clean:
	${RM} $(OBJECTS) $(DEBUGOBJECTS) ptracer.exe rabbit.exe *~ core

.PHONY: all clean
