## Makefile
##
## @author: Lothar Rubusch
## @email: L.Rubusch@gmx.ch
## @license: GPLv3 2013
##
## use -O, -O1, -O2 for optimization


SOURCE = ptrace.c
ASMSOURCE = dummy1.s
OBJECTS = $(SOURCE:%.c=%.o)
CC = gcc
CFLAGS += -g -Wall #-std=c99 -pedantic
RM = rm -f

%.o: %.c
	$(CC) -c $(CFLAGS) $<


all: ptrace.exe dummy1.exe

ptrace.exe: $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) -lm

dummy1.exe:
	${CC} -o $@ $(ASMSOURCE)


clean:
	$(RM) $(OBJECTS) ptrace.exe dummy1.exe *~

.PHONY: clean all
