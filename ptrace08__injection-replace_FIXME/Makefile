## Makefile
##
## @author: Lothar Rubusch
## @email: L.Rubusch@gmx.ch
## @license: GPLv3 2013
##
## use -O, -O1, -O2 for optimization


SOURCE = ptracer.c rabbit.c hello.c
OBJECTS = $(SOURCE:%.c=%.o)
CC = gcc
CFLAGS += -g -Wall #-std=c99 -pedantic
RM = rm -f

%.o: %.c
	$(CC) -c $(CFLAGS) $<


all: ptracer.exe rabbit.exe hello.exe

ptracer.exe: $(OBJECTS)
	$(CC) -o $@ ptracer.o -lm

rabbit.exe: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ rabbit.o -lm

hello.exe: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ hello.o -lm

clean:
	$(RM) $(OBJECTS) ptracer.exe rabbit.exe hello.exe *~

.PHONY: clean all
