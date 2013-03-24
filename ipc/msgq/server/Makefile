# Makefile

SRC = server.c
OBJ = $(SRC:.c=.o)
CC = gcc
CFLAGS += -g -Wall -D _XOPEN_SOURCE=600 -D _GNU_SOURCE
INCLUDES += -I./ -I../
RM = rm -f

%.o: %.c
	$(CC) $(INCLUDES) -c $(CFLAGS) $<

server.exe: $(OBJ)
	$(CC) -o $@ $(OBJ) -lm

all: server.exe

clean:
	$(RM) $(OBJ) server.exe *~

.PHONY: all clean
