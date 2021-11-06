SOURCE = callback.c
TARGET = ${SOURCE:.c=.exe}
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -Werror -std=gnu99
RM = rm -f

.c.o:
	${CC} ${CFLAGS} -c $<

${TARGET}: ${OBJECTS}
	${CC} ${CFLAGS} -o $@ ${OBJECTS} -lm

all:	${TARGET}

clean:
	${RM} ${OBJECTS} ${TARGET} *~
