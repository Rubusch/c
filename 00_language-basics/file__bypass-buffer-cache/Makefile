SOURCE = filecache.c
OBJECTS = $(SOURCE:.c=.o)
TARGET = $(SOURCE:.c=.elf)

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -Werror -std=gnu99 -pedantic
RM = rm -f

.c.o:
	$(CC) $(CFLAGS) -c $<

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) tlpi/get_num.c -o $@ $(OBJECTS)

all: $(TARGET)

clean:
	-$(RM) $(TARGET) *~ *.o

.PHONY: all clean
