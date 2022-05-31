SOURCE = catching_sighup.c
OBJECTS = $(SOURCE:%.c=%.o)
TARGET = $(SOURCE:%.c=%.elf)

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
	-$(RM) $(TARGET) $(OBJECTS) *~ *.log

.PHONY: clean all
