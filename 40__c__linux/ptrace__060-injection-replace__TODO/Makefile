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
## NOTE: rabbit.o: relocation R_X86_64_32S against `.text' can not be used when making a PIE object; recompile with -fPIC
## NOTE: -pedantic: a member of a structure or union cannot have a variably modified type (used to convert long into char array)
CFLAGS += -g -Wall -fPIC -no-pie -std=gnu99
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

rabbit-XXX.exe:
	$(CC) $(CFLAGS) -c rabbit-XXX.c
	$(CC) $(CFLAGS) -o $@ rabbit-XXX.o

clean:
	$(RM) $(OBJECTS) ptracer.exe rabbit.exe rabbit-XXX.exe rabbit.o rabbit-XXX.o *~

.PHONY: clean all
