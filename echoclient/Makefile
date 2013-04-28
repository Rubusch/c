## Makefile
## @author: Lothar Rubusch
## @email: L.Rubusch@gmx.ch
## @license: GPLv3
## @date: 2013-april-28

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