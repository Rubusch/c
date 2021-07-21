# Makefile (simple)
#
# use -O, -O1, -O2 for optimization


SOURCE = fork.c
TARGET = $(SOURCE:.c=.exe)
OBJECTS = $(SOURCE:.c=.o)

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
	$(RM) $(OBJECTS) $(TARGET) *~

.PHONY: all clean
