# Makefile

SOURCE = server.c
OBJECTS = $(SOURCE:.c=.o)
TARGET = $(SOURCE:.c=.exe)

CC = gcc
CFLAGS = -g -Wall -Werror -std=c11 -pedantic
RM = rm -f

%.o: %.c
	$(CC) $(CFLAGS) -c $<

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) -lm

clean:
	$(RM) $(OBJECTS) $(TARGET) *~

.PHONY: all clean
