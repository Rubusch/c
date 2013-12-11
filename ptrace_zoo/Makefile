## Makefile
##
## @author: Lothar Rubusch
## @email: L.Rubusch@gmx.ch
## @license: GPLv3 2013
##
## use -O, -O1, -O2 for optimization


SOURCE = rabbit_hello.c rabbit_runner.c
OBJECTS = $(SOURCE:%.c=%.o)
CC = gcc
CFLAGS += -g -Wall #-std=c99 -pedantic
RM = rm -f

%.o: %.c
	$(CC) -c $(CFLAGS) $<


all: rabbit_hello.exe rabbit_runner.exe

rabbit_hello.exe: $(OBJECTS)
	$(CC) -o $@ rabbit_hello.o -lm

rabbit_runner.exe: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ rabbit_runner.o -lm

clean:
	$(RM) $(OBJECTS) rabbit_hello.exe rabbit_runner.exe *~

.PHONY: clean all
