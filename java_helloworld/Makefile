# Makefile

SRC = start.c
OBJ = $(SRC:.c=.o)
CC = gcc
CFLAGS = -g -Wall
RM = rm -f

%.o: %.c
	$(CC) -c $(CFLAGS) $<

start.exe: $(OBJ)
	$(CC) -o $@ $(OBJ) -lm

all: start.exe

clean:
	$(RM) $(OBJ) start.exe *~

.PHONY: all clean