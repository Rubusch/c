## Makefile
##
## @author: Lothar Rubusch
## @email: L.Rubusch@gmx.ch
## @license: GPLv3 2013
##
## use -O, -O1, -O2 for optimization


LONG_BIT := $(shell getconf LONG_BIT)
#SOURCE = ptracer.c
#ASMSOURCE = rabbit.s
# TODO: use .c files instead of asm files
SOURCE = ptracer.c rabbit.c

OBJECTS = $(SOURCE:%.c=%.o)
CC = gcc
## NOTE: don't use -std=c99!!! -std=c99 disables "asm volatile", since it is not standard C99,
## either use '__asm__' instead of 'asm' with '-std=c99', or use '-std=gnu99'
CFLAGS += -g -Wall -fPIC -pedantic -std=gnu99
RM = rm -f

%.o: %.c
	$(CC) $(CFLAGS) -c $<


all: ptracer.exe rabbit.exe

ptracer.exe: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ ptracer.o -lm

rabbit.exe: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ rabbit.o -lm
	@echo ""
	@echo "start the example by:"
	@echo '$$ ./rabbit.exe & ./ptracer.exe $$(pidof rabbit.exe)'

clean:
	$(RM) $(OBJECTS) ptracer.exe rabbit.exe rabbit.o *~

.PHONY: clean all
