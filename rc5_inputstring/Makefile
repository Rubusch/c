# Makefile (simple)
#
# use -O, -O1, -O2 for optimization


SOURCE = rc5input.c
OBJECTS = $(SOURCE:%.c=%.o)
CC = gcc
CFLAGS += -g # -Wchar-subscripts
#CFLAGS += -g -Wall -std=c99 -pedantic
RM = rm -f

%.o: %.c
	$(CC) -c $(CFLAGS) $<

rc5input.exe: $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) -lm

all: rc5input.exe

clean:
	$(RM) $(OBJECTS) rc5input.exe *~

.PHONY: clean all
