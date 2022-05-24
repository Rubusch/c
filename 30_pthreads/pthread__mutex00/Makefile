# Makefile (simple)
#
# use -O, -O1, -O2 for optimization


SOURCE = arrays.c
TARGET = $(SOURCE:.c=.elf)
OBJECTS = $(SOURCE:.c=.o)
CC = gcc
CFLAGS += -g -Wall -std=c99 -pedantic -Werror
RM = rm -f

%.o: %.c
	$(CC) $(CFLAGS) -c $<

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) -pthread

all: $(TARGET)

clean:
	$(RM) $(OBJECTS) $(TARGET) *~


