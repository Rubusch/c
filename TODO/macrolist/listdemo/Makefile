# Makefile
SOURCE = main.c dl_list.c lib/consoleinput.c
HEADERS = dl_list.h lib/consoleinput.h
TARGET = dl_list.exe
OBJECTS = $(SOURCE:.c=.o)
.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -std=c99 -pedantic -Werror
RM = rm -f

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) -lm

all: $(TARGET)

.PHONY: clean

clean:
	$(RM) $(TARGET) *~ *.o ./lib/*~ ./lib/*.o core
