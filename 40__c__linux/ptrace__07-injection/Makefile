## Makefile
##
## @author: Lothar Rubusch
## @email: L.Rubusch@gmx.ch
## @license: GPLv3 2013
##
## use -O, -O1, -O2 for optimization


LONG_BIT := $(shell getconf LONG_BIT)
SOURCE = ptracer.c
# TODO: use .c files instead of asm files
#SOURCE = ptrace.c rabbit.c
ASMSOURCE = rabbit.s
OBJECTS = $(SOURCE:%.c=%.o)
CC = gcc
CFLAGS += -g -Wall -fPIC -std=c99 -pedantic
RM = rm -f


%.o: %.c
	$(CC) -c $(CFLAGS) $<


all: ptracer.exe rabbit.exe

ptracer.exe: $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) -lm

ifeq ($(LONG_BIT),64)
ifeq (, $(shell which nasm))
$(error "nasm is not installed, please install nasm")
endif
rabbit.exe:
	nasm -f elf64 -o rabbit.o $(ASMSOURCE)
	ld -o $@ rabbit.o
else
rabbit.exe: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ rabbit.o -lm
endif
	@echo ""
	@echo "start the example by:"
	@echo '$$ ./rabbit.exe & ./ptracer.exe $$(pidof rabbit.exe)'


clean:
	$(RM) $(OBJECTS) ptracer.exe rabbit.exe rabbit.o *~

.PHONY: clean all
