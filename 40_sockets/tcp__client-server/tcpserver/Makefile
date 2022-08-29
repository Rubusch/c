SOURCE = server.c
OBJECTS = $(SOURCE:.c=.o)
TARGET = $(SOURCE:.c=.exe)
INC += -I../

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -Werror -std=gnu99 -pedantic $(INC)
RM = rm -f

.PHONY: $(TARGET) clean

%.c%.o:
	$(CC) -c $(CFLAGS) $<

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) ../tlpi/get_num.c -o $@ $(OBJECTS)

clean:
	$(RM) $(OBJECTS) $(TARGET) *~
