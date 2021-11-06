# Makefile (simple)
#
# use -O, -O1, -O2 for optimization

SOURCE = signalhandler01.c
TARGET = $(SOURCE:.c=.exe)
OBJECTS = $(SOURCE:.c=.o)
CC = gcc
CFLAGS += -g -Wall -std=gnu99 -pedantic -Werror
RM = rm -f

%.o: %.c
	$(CC) $(CFLAGS) -c $<

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) -lm -pthread

all: $(TARGET)

clean:
	$(RM) $(OBJECTS) $(TARGET) *~

.PHONY: all clean
