## Makefile
##
## @author: Lothar Rubusch
## @email: L.Rubusch@gmx.ch
## @license: GPLv3 2013
##
## $ ./rabbit.exe & ./ptracer.exe $(pidof rabbit.exe) 0x55d107487000
## [1] 24380
## counter: '0'
## tracing pid 24380. checking for change to 0x0000000007487000
## DEBUG main()[64]:       0x0000000000000080 - ipoffs
## DEBUG main()[67]:       0x0000000000000098 - spoffs
## Attaching to process 24380
## DEBUG main()[83]:       wait result stat 0x000000000000137f, pid 24380
## DEBUG main()[91]:       starting value at 0x0000000007487000, is 0x00000000ffffffff
## DEBUG main()[110]:      0x0000000000000005 - signo (SIGTRAP)
## press ENTER

SOURCE = ptracer.c rabbit.c
OBJECTS = $(SOURCE:%.c=%.o)
CC = gcc
CFLAGS += -g -Wall -std=c99 -pedantic
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
	@echo '$$ ./rabbit.exe & ./ptracer.exe $$(pidof rabbit.exe) 0x55d107487000'

clean:
	$(RM) $(OBJECTS) ptracer.exe rabbit.exe *~

.PHONY: clean all
