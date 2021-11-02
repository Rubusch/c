## Makefile
##
## @author: Lothar Rubusch
## @email: L.Rubusch@gmx.ch
## @license: GPLv3

SOURCE = endian.c
OBJECTS = $(SOURCE:.c=.o)
TARGET = $(SOURCE:.c=.exe)

.PREFIXES = .c .o
CC = gcc
CFLAGS = -O -g -Wall -std=c99
RM = rm -f

.c.o:
	$(CC) -c $(CFLAGS) $<

$(TARGET): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS)

all: $(TARGET)

clean:
	$(RM) $(TARGET) *~ *.o

.PHONY: all clean
