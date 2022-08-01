SOURCE = utmpx.c
TARGET = $(SOURCE:.c=.elf)
OBJECTS = $(SOURCE:.c=.o)
.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -std=gnu99 -pedantic -Werror
RM = rm -f

.c.o:
	$(CC) $(CFLAGS) -c $<

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) tlpi/get_num.c -o $@ $(OBJECTS)

all: $(TARGET)

clean:
	$(RM) $(OBJECTS) $(TARGET) *~

.PHONY: clean all
