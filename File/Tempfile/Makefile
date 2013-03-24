# Makefile
#
#

SOURCE = tempfile.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -std=c99
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

tf: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: tf

clean:
	${RM} tf.exe ${OBJECTS} *~ *.txt core
