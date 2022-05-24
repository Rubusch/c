SOURCE = pthread_mutex.c
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


