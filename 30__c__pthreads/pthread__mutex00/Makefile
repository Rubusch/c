# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 

SOURCE = arrays.c
OBJECTS = $(SOURCE:.c=.o)

CC = gcc
CFLAGS += -g -Wall

%.o: %.c
	$(CC) -c $(CFLAGS) $<

arrays.exe: $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) -lm -pthread

.PHONY: all
all: arrays.exe

.PHONY: clean
clean: 
	$(RM) $(OBJECTS) arrays.exe *~


