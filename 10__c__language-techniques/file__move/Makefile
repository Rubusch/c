# Makefile
SOURCE = filemove.c
OBJECTS = ${SOURCE:.c=.o}
TARGET = ${SOURCE:.c=.exe}
.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -Werror -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} ${CFLAGS} -c $<

${TARGET}: ${OBJECTS}
	cp -f template.txt text.txt
	${CC} ${CFLAGS} -o $@ ${OBJECTS} -lm

all: ${TARGET}

clean:
	-${RM} ${TARGET} ${OBJECTS} Mickey-Mouse.txt *~
