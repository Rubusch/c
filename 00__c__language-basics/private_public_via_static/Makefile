# Makefile
#
#
TARGET = scope.exe

SOURCE = main.c foo.c
HEADERS = foo.h
OBJECTS = $(SOURCE:.c=.o)

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -Werror -std=c89 -pedantic
RM = rm -f

.c.o:
	$(CC) $(CFLAGS) -c $<

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) -lm

all: $(TARGET)

clean:
	$(RM) $(TARGET) $(OBJECTS) *~ *.txt core

