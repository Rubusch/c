# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 

TARGET = getip.exe
SRC = getip.c
OBJ = ${SRC:%.c=%.o}

#.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall
RM = rm -f

## product
all: $(TARGET)
.PHONY: all

## clean
clean: 
	${RM} $(OBJ) getip.exe *~ core

## compile
%.c: %.o
	${CC} ${CFLAGS} -c $<

$(TARGET): ${OBJ}
	${CC} -o $@ ${OBJ} -lm -pthread



