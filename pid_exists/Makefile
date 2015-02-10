# Makefile

SRC = pid_exists.c
OBJ = $(SRC:.c=.o)
CC = gcc
CFLAGS = -g -Wall
RM = rm -f

%.o: %.c
	$(CC) -c $(CFLAGS) $<

pid_exists.exe: $(OBJ)
	$(CC) -o $@ $(OBJ) -lm

all: pid_exists.exe

clean:
	$(RM) $(OBJ) pid_exists.exe *~

.PHONY: all clean