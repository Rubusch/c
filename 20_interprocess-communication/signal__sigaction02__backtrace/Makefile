## NB: this needs '-rdynamic' to be set to show the function names
##
SOURCE = sigaction.c
OBJECTS = $(SOURCE:.c=.o)
TARGET = $(SOURCE:.c=.exe)

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -rdynamic -Wall -Werror -std=gnu11 -pedantic
RM = rm -f

%.c%.o:
	$(CC) -c $(CFLAGS) $<


all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)


clean:
	$(RM) $(OBJECTS) $(TARGET) *~

.PHONY: $(TARGET) clean

