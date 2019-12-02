## Makefile
##
## @author: Lothar Rubusch
## @email: L.Rubusch@gmx.ch
## @license: GPLv3 2013
##
## use -O, -O1, -O2 for optimization
SOURCE = ptracer.c
OBJECTS = $(SOURCE:%.c=%.o)
TARGET = $(SOURCE:%.c=%.exe)
CC = gcc
CFLAGS += -g -Wall -std=c99 -pedantic
RM = rm -f

%.o: %.c
	$(CC) -c $(CFLAGS) $<


all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) -lm


clean:
	$(RM) $(OBJECTS) $(TARGET) *~

.PHONY: clean all
