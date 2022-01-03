## Makefile (simple)
##
## @author: Lothar Rubusch
## @email: L.Rubusch@gmx.ch
## @license: GPLv3
##
## use -O, -O1, -O2 for optimization


SOURCE = bitoperations.c lotharlib/console_input.c
TARGET = bitoperations.exe
HEADERS = lotharlib/console_input.h
OBJECTS = $(SOURCE:.c=.o)

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -std=c99 -pedantic
RM = rm -f

.c.o:
	$(CC) -o $@ -c $(CFLAGS) $<

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS)

clean:
	$(RM) $(OBJECTS) $(TARGET) *~


