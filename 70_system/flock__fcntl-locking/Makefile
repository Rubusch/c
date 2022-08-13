SOURCE = flock.c
OBJECTS = $(SOURCE:.c=.o)
TARGET = $(SOURCE:.c=.elf)
.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -Werror -pedantic
RM = rm -f

%.c%.o:
	$(CC) -c $(CFLAGS) $<


all: $(TARGET)

$(TARGET): $(OBJECTS)
	truncate -s 100 tfile
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)


clean:
	$(RM) $(OBJECTS) $(TARGET) tfile *~

.PHONY: $(TARGET) clean

