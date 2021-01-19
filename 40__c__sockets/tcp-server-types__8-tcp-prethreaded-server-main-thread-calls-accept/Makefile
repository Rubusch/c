SOURCE = server.c
OBJECTS = $(SOURCE:.c=.o)
TARGET = $(SOURCE:.c=.exe)

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -Werror -std=c11 -pedantic
RM = rm -f

%.c%.o:
	$(CC) $(CFLAGS) -o $@ -c $<

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) -lm -pthread

clean:
	$(RM) $(OBJECTS) $(TARGET) *~ core

.PHONY: all clean
