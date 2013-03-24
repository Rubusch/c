# Makefile

SRC = client.c
OBJ = $(SRC:.c=.o)
CC = gcc
CFLAGS += -g -Wall -D _XOPEN_SOURCE=600
INCLUDES += -I./ -I../
RM = rm -f

%.o: %.c
	$(CC) $(INCLUDES) -c $(CFLAGS) $<

client.exe: $(OBJ)
	$(CC) -o $@ $(OBJ) -lm

all: client.exe

clean:
	$(RM) $(OBJ) client.exe *~

.PHONY: all clean