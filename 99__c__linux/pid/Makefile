# Makefile

SRC = pid_exists.c
TARGET = $(SRC:.c=.exe)
OBJ = $(SRC:.c=.o)
CC = gcc
CFLAGS = -g -Wall -Werror -pedantic
RM = rm -f

%.o: %.c
	$(CC) $(CFLAGS) -c $<

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) -lm

all: $(TARGET)

clean:
	$(RM) $(OBJ) $(TARGET) *~

.PHONY: all clean
