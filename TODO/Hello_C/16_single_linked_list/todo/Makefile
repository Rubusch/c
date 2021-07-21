# Makefile
#
#

SOURCE = main.c sl_list.c
HEADERS = sl_list.h
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -O -g -Wall -std=c99
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

sl_list.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: sl_list.exe

.PHONY: clean

clean: 
	${RM} sl_list.exe *~ *.o core


