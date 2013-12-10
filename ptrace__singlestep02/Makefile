## Makefile
##
## @author: Lothar Rubusch
## @email: L.Rubusch@gmx.ch
## @license: GPLv3 2013
##
## use -O, -O1, -O2 for optimization


SOURCE = ptracer.c rabbit.c
OBJECTS = $(SOURCE:%.c=%.o)
CC = gcc
#CFLAGS += -g -Wall #-std=c99 -pedantic
CFLAGS += -W -Wall -O3 -fomit-frame-pointer
RM = rm -f

%.o: %.c
	$(CC) -c $(CFLAGS) $<


all: ptracer.exe rabbit.exe

ptracer.exe: $(OBJECTS)
#	$(CC) -o $@ ptracer.o -lm
	$(CC) $(CFLAGS) -o $@ ptracer.o

rabbit.exe: $(OBJECTS)
#	$(CC) $(CFLAGS) -o $@ rabbit.o -lm
	$(CC) $(CFLAGS) -o $@ rabbit.o -pthread

clean:
	$(RM) $(OBJECTS) ptracer.exe rabbit.exe *~

.PHONY: clean all
