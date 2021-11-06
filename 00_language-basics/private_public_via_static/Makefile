SOURCE = main.c foo.c
HEADERS = foo.h
OBJECTS = $(SOURCE:.c=.o)
TARGET = scope.exe

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -Werror -std=c11 -pedantic
RM = rm -f

%.c%.o:
	$(CC) $(CFLAGS) -c $<

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) -lm

clean:
	$(RM) $(TARGET) $(OBJECTS) *~

.PHONY: all clean
