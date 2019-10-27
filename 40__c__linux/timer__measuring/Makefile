# Makefile (simple)
#
# use -O, -O1, -O2 for optimization


SOURCE = timemeasuring.c
TARGET = $(SOURCE:.c=.exe)
OBJECTS = $(SOURCE:.c=.o)
.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -Werror -pedantic
RM = rm -f

.c.o:
	$(CC) $(CFLAGS) -c $<

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) -lm -lrt
# linked against librt for the time.h functions -> -lrt

all: $(TARGET)

clean: 
	$(RM) $(OBJECTS) $(TARGET) *~ core


