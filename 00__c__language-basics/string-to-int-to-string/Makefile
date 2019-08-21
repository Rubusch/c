## Makefile
##
## .h and .c files

SOURCE = string2int.c
OBJECTS = $(SOURCE:%.c=%.o)
CC = gcc
CFLAGS += -g -Wall -Werror -std=gnu99 -pedantic
RM = rm -f

%.o: %.c
	$(CC) $(CFLAGS) -c $<

string2int.exe: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) -lm

all:	string2int.exe

clean:
	$(RM) $(OBJECTS) string2int.exe *~

.PHONY: all clean
