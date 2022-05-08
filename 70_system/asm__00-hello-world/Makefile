SOURCE = rabbit.c
OBJECTS = $(SOURCE:%.c=%.o)
TARGET = $(SOURCE:%.c=%.elf)
CC = gcc
CFLAGS += -g -Wall -std=gnu99 -pedantic
RM = rm -f

%.o: %.c
	$(CC) $(CFLAGS) -c $<

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $<

clean:
	$(RM) $(OBJECTS) $(TARGET) *~

.PHONY: clean all
