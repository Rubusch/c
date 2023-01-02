SOURCE = semctl.c console_input.c
TARGET = semctl.elf
OBJECTS = $(SOURCE:.c=.o)
HEADERS = console_input.h
.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -Werror -pedantic
RM = rm -f

.c.o:
	$(CC) $(CFLAGS) -c $<

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

clean:
	$(RM) $(OBJECTS) $(TARGET) *~
