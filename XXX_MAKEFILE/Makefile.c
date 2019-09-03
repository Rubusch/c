## Makefile (simple)
##
## @author: Lothar Rubusch
## @email: L.Rubusch@gmx.ch
## @license: GPLv3
##
## use -O, -O1, -O2 for optimization

TARGET = name.exe

SRC = name.c
OBJ = $(SRC:%.c=%.o)
CC = gcc
CFLAGS += -g -Wall -Werror -standard=c99 -pedantic
RM = rm -f

%.c: %.o
	$(CC) $(CFLAGS) -c $<


all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) -lm 

clean:
	$(RM) $(OBJ) $(TARGET) *~

.PHONY: clean all
