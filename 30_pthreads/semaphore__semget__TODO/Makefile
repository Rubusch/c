SOURCE = semget.c
TARGET = $(SOURCE:.c=.elf)
OBJECTS = $(SOURCE:.c=.o)
.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -Werror -pedantic
RM = rm -f

.c.o:
	$(CC) $(CFLAGS) -c $<

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) -D _SVID_SOURCE -D _XOPEN_SOURCE

all: $(TARGET)

clean:
	$(RM) $(OBJECTS) $(TARGET) *~
