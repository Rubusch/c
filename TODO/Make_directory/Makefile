# Makefile
#
# .h and .c files

SOURCE = main.c directory/folder.c
HEADERS = directory/folder.h
TARGET = $(SOURCE:.c=.exe)
OBJECTS = $(SOURCE:.c=.o)

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -std=gnu99
RM = rm

.c.o:
	$(CC) -o $@ -c $(CFLAGS) $<

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) -lm

all:	$(TARGET)

clean:
	$(RM) $(OBJECTS) $(TARGET) *~


