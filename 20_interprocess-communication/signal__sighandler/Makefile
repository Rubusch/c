SOURCE = signalhandler.c
OBJECTS = $(SOURCE:.c=.o)
TARGET = $(SOURCE:.c=.exe)

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -Werror -std=gnu99 -pedantic
RM = rm -f

%.c%.o:
	$(CC) -c $(CFLAGS) $<


all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)


clean:
	$(RM) $(OBJECTS) $(TARGET) *~

.PHONY: $(TARGET) clean

