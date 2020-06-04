# Makefile
#
#

TARGET = memfoo.exe
SOURCE = memfoo.c
OBJECTS = $(SOURCE:.c=.o)

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -std=c99 -pedantic
RM = rm -f

.c.o:
	$(CC) -c $(CFLAGS) $<

$(TARGET): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) -lm

all: $(TARGET)

clean:
	$(RM) $(TARGET) *.o *~ core


