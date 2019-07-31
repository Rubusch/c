# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = restaurant.c
OBJECTS = $(SOURCE:.c=.o)

CC = gcc
CFLAGS += -g -Wall -std=c99 -pedantic 
RM = rm -f

%.o: %.c
	$(CC) -c $(CFLAGS) $<

restaurant.exe: $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) -lm -lpthread -D _XOPEN_SOURCE

.PHONY: all
all: restaurant.exe

.PHONY: clean
clean: 
	$(RM) $(OBJECTS) restaurant.exe *~
