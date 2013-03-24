# Makefile

SRC = echoclient.c
OBJ = $(SRC:.c=.o)
CC = gcc
CFLAGS = -g -Wall
RM = rm -f

%.o: %.c
	$(CC) -c $(CFLAGS) $<

echoclient.exe: $(OBJ)
	$(CC) -o $@ $(OBJ) -lm

all: echoclient.exe

clean:
	$(RM) $(OBJ) echoclient.exe *~

.PHONY: all clean