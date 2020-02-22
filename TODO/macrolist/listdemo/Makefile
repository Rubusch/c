# Makefile
#
#

SOURCE = main.c dl_list.c lib/consoleinput.c
HEADERS = dl_list.h lib/consoleinput.h
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} -o $@ -c ${CFLAGS} $<

dl_list.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: dl_list.exe

.PHONY: clean

clean: 
	${RM} dl_list.exe *~ *.o ./lib/*~ ./lib/*.o core


