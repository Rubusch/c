## Makefile (simple)
##
## @author: Lothar Rubusch <L.Rubusch@gmail.com>
## @license: GPLv3
TARGET = can_demo.exe
SOURCE = main.c can_interface.c fifo.c
OBJECTS = $(SOURCE:.c=.o)
FIFO_SOURCE = fifo/fifo.c
.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -std=c99 -pedantic -Werror
RM = rm -f

.c.o:
	$(CC) $(CFLAGS) -c fifo/fifo.c $(FIFO_SOURCE)
	$(CC) $(CFLAGS) -c $<

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) -lm

all: $(TARGET)

clean:
	$(RM) $(OBJECTS) $(TARGET) *~ *.log core
