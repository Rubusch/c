SOURCE = syscall.c
OBJECTS = $(SOURCE:%.c=%.o)
TARGET = $(SOURCE:%.c=%.elf)
CC = gcc
CFLAGS += -g -Wall -std=c99 -pedantic
RM = rm -f

%.o: %.c
	$(CC) $(CFLAGS) -c $<

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

clean:
	$(RM) $(OBJECTS) $(TARGET) *~

.PHONY: clean all
