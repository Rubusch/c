# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 

SOURCE = rwlock.c
OBJECTS = $(SOURCE:.c=.o)

CC = gcc
CFLAGS += -g -Wall

%.o: %.c
	$(CC) -c $(CFLAGS) $<

rwlock.exe: $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) -lm -pthread

.PHONY: all
all: rwlock.exe

.PHONY: clean
clean: 
	$(RM) $(OBJECTS) rwlock.exe *~


