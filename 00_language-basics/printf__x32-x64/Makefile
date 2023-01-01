SOURCE = x64.c
OBJECTS = $(SOURCE:.c=.o)
TARGET = $(SOURCE:.c=.elf)

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -Werror -std=gnu99 -pedantic
RM = rm -f

%.c%.o:
	$(CC) $(CFLAGS) -c $<

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

clean:
	$(RM) $(TARGET) $(OBJECTS) *~

.PHONY: all clean
