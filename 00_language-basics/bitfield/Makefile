SOURCE = bitfield.c
OBJECTS = $(SOURCE:.c=.o)
TARGET = $(SOURCE:.c=.elf)
.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -std=c99
RM = rm -f

.c.o:
	$(CC) -c $(CFLAGS) $<

$(TARGET): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS)

all: $(TARGET)

clean:
	$(RM) $(TARGET) *~ *.o

.PHONY: all clean
