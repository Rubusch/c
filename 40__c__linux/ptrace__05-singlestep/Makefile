## Makefile
##
## @author: Lothar Rubusch
## @email: L.Rubusch@gmx.ch
## @license: GPLv3 2013
##
## use -O, -O1, -O2 for optimization

SOURCE = ptracer.c
LONG_BIT := $(shell getconf LONG_BIT)
ifeq ($(LONG_BIT),64)
ASMSOURCE = bunny.s
else
ASMSOURCE = rabbit.s
endif
OBJECTS = $(SOURCE:%.c=%.o)
CC = gcc
CFLAGS += -g -Wall -fPIC #-std=c99 -pedantic
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
rabbit.exe:
	${CC} -o $@ $(ASMSOURCE)
endif
	@echo ""
	@echo "start the example by:"
	@echo "$$ ./ptracer.exe"

clean:
	$(RM) $(OBJECTS) ptracer.exe rabbit.exe rabbit.o *~

.PHONY: clean all
