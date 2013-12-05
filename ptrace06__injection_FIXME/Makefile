## Makefile
##
## @author: Lothar Rubusch
## @email: L.Rubusch@gmx.ch
## @license: GPLv3 2013
##
## use -O, -O1, -O2 for optimization


SOURCE = ptrace.c rabbit.c
OBJECTS = $(SOURCE:%.c=%.o)
CC = gcc
CFLAGS += -g -Wall #-std=c99 -pedantic
RM = rm -f

%.o: %.c
	$(CC) -c $(CFLAGS) $<


all: ptrace.exe rabbit.exe

# TODO rm
#ptrace.exe: $(OBJECTS)
#	$(CC) -o $@ $(OBJECTS) -lm

ptrace.exe: $(OBJECTS)
	$(CC) -o $@ ptrace.o -lm

rabbit.exe: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ rabbit.o -lm

clean:
	$(RM) $(OBJECTS) ptrace.exe rabbit.exe *~

.PHONY: clean all
