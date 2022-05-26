SOURCE = pthread_join.c
TARGET = $(SOURCE:.c=.elf)
OBJECTS = $(SOURCE:.c=.o)
CC = gcc
CFLAGS += -g -Wall -std=gnu99 -pedantic -Werror
RM = rm -f

%.o: %.c
	$(CC) $(CFLAGS) -c $<

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) -pthread

all: $(TARGET)

clean:
	$(RM) $(OBJECTS) $(TARGET) *~

.PHONY: all clean
