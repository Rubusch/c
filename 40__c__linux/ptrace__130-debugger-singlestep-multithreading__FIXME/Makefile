## Makefile
##
## @author: Lothar Rubusch
## @email: L.Rubusch@gmx.ch
## @license: GPLv3 2013

SOURCE = ptracer.c rabbit.c
OBJECTS = $(SOURCE:%.c=%.o)
CC = gcc
CFLAGS += -W -Wall -O3 -fomit-frame-pointer
RM = rm -f

%.o: %.c
	$(CC) $(CFLAGS) -c $<


all: ptracer.exe rabbit.exe

ptracer.exe: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ ptracer.o

rabbit.exe: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ rabbit.o -pthread

clean:
	$(RM) $(OBJECTS) ptracer.exe rabbit.exe *~

.PHONY: clean all
