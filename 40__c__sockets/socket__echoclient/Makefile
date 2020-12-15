## Makefile
## @author: Lothar Rubusch
## @email: L.Rubusch@gmx.ch
## @license: GPLv3
## @date: 2013-april-28

SOURCE = echoclient.c
OBJECTS = $(SOURCE:.c=.o)
TARGET = $(SOURCE:.c=.exe)

CC = gcc
CFLAGS = -g -Wall -Werror -pedantic
RM = rm -f

%.o: %.c
	$(CC) $(CFLAGS) -c $<

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) -lm

all: $(TARGET)

clean:
	$(RM) $(OBJECTS) $(TARGET) *~

.PHONY: all clean
