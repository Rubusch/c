## Makefile
##
## .h and .c files

SOURCE = string2int.c
OBJECTS = $(SOURCE:%.c=%.o)
CC = gcc
CFLAGS += -g -Wall -std=gnu99 
RM = rm -f

%.o: %.c
	$(CC) -c $(CFLAGS) $<

string2int.exe: $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) -lm

all:	string2int.exe

clean:
	$(RM) $(OBJECTS) string2int.exe *~

.PHONY: all clean
