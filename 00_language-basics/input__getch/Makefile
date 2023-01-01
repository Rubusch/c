SOURCE = getch.c
TARGET = opengetch.elf
OBJECTS = $(SOURCE:.c=.o)

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -Werror -std=gnu99 -pedantic
RM = rm -f

.c.o:
	$(CC) $(CFLAGS) -c $<

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

all: $(TARGET)

clean:
	-$(RM) $(TARGET) *.o *~

.PHONY: clean all
