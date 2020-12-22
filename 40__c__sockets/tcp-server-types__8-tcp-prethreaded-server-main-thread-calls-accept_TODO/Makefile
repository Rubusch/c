# Makefile (simple)

SOURCE = server.c thread.c
OBJECTS = $(SOURCE:.c=.o)
TARGET = server.exe

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -Werror -pedantic
RM = rm -f

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) -lm -pthread

all: $(TARGET)

clean:
	$(RM) $(OBJECTS) $(TARGET) *~ core
