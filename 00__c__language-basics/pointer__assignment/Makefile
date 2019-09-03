# Makefile
#
#

TARGET = pointer.exe

SOURCE = pointer.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -Werror -pedantic -std=c99
RM = rm -f

.c.o:
	${CC} ${CFLAGS} -c ${SOURCE} $<

$(TARGET): ${OBJECTS}
	${CC} ${CFLAGS} -o $@ ${OBJECTS} -lm

all: $(TARGET)

clean:
	${RM} $(TARGET) $(OBJECTS) *~ core
