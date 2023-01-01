SOURCE = alignof.c
OBJECTS = $(SOURCE:.c=.o)
TARGET = $(SOURCE:.c=.elf)

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -Werror -std=c11 -pedantic
RM = rm -f

.c.o:
	$(CC) $(CFLAGS) -c $(SOURCE) $<

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

all: $(TARGET)

clean:
	-$(RM) $(TARGET) *.o *~
