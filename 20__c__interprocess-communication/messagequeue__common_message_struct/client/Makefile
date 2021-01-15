SOURCE = client.c
OBJECTS = $(SOURCE:.c=.o)
TARGET = $(SOURCE:.c=.exe)
INCLUDES += -I../

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -Werror -std=c99 -pedantic
RM = rm -f

%.c%.o:
	$(CC) $(INCLUDES) -c $(CFLAGS) $<


all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) -lm -D _XOPEN_SOURCE=600 -D _GNU_SOURCE


clean:
	$(RM) $(OBJECTS) $(TARGET) *~

.PHONY: $(TARGET) clean

