SOURCE = scratchy.c
OBJECTS = $(SOURCE:.c=.o)
TARGET = $(SOURCE:.c=.elf)

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -Werror -std=c99 -pedantic
RM = rm -f

%.c%.o:
	$(CC) -c $(CFLAGS) $<


all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) -lm


clean:
	$(RM) $(OBJECTS) $(TARGET) *~

.PHONY: $(TARGET) clean

