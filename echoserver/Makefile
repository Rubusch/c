# Makefile

SRC = echoserver.c
OBJ = $(SRC:.c=.o)
CC = gcc
CFLAGS = -g -Wall
RM = rm -f

%.o: %.c
	$(CC) -c $(CFLAGS) $<

echoserver.exe: $(OBJ)
	$(CC) -o $@ $(OBJ) -lm

all: echoserver.exe

clean:
	$(RM) $(OBJ) echoserver.exe *~

.PHONY: all clean