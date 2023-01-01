SOURCE = main.c console_input.c
TARGET = input.elf
#HEADERS = console_input.h
OBJECTS = $(SOURCE:.c=.o)

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -Werror -std=gnu99
RM = rm -f

.c.o:
	$(CC) $(CFLAGS) -c $<

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

all:	$(TARGET)

clean:
	$(RM) $(OBJECTS) $(TARGET) *~

.PHONY: all clean
