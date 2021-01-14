SOURCE = scratchy.c ../lotharlib/console_input.c
OBJECTS = $(SOURCE:.c=.o)
TARGET = scratchy.exe
INC += -I../lotharlib/

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -std=c99 -pedantic
RM = rm -f

%.c%.o:
	$(CC) $(INC) -c $(CFLAGS) $<


all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(INC) $(CFLAGS) -o $@ $(OBJECTS) -lm


clean:
	$(RM) $(OBJECTS) $(TARGET) *~

.PHONY: $(TARGET) clean

