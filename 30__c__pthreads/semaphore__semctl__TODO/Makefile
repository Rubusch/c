# Makefile (simple)
#
# use -O, -O1, -O2 for optimization

SOURCE = semctl.c console_input.c
TARGET = semctl.exe
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
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) -lm


clean:
	$(RM) $(OBJECTS) $(TARGET) *~ core
