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
CFLAGS += -g -Wall -std=c99 #-pedantic
RM = rm -f

%.o: %.c
	$(CC) $(CFLAGS) -c $<


all: ptracer.exe rabbit.exe

ptracer.exe: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ ptracer.o -lm

rabbit.exe: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ rabbit.o
	@echo ""
	@echo "start the example by:"
	@echo '$$ ./rabbit.exe & ./ptracer.exe $$(pidof rabbit.exe)'

clean:
	$(RM) $(OBJECTS) ptracer.exe rabbit.exe *~

.PHONY: clean all
