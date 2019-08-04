## Makefile
##
## @author: Lothar Rubusch
## @email: L.Rubusch@gmx.ch
## @license: GPLv3 2013
##
## use -O, -O1, -O2 for optimization


SOURCE = ptracer.c
OBJECTS = $(SOURCE:%.c=%.o)
CC = gcc
CFLAGS += -g -Wall #-std=c99 -pedantic
RM = rm -f

%.o: %.c
	$(CC) -c $(CFLAGS) $<


all: ptracer.exe

ptracer.exe: $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) -lm


clean:
	$(RM) $(OBJECTS) ptracer.exe *~

.PHONY: clean all
