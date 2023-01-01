SOURCE = test__main.c
TARGET = ${SOURCE:.c=.elf}
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -Werror
RM = rm -f

.c.o:
	${CC} ${CFLAGS} -c $<

${TARGET}: ${OBJECTS}
	${CC} ${CFLAGS} -o $@ ${OBJECTS} -lcunit

all:	${TARGET}

clean:
	${RM} ${OBJECTS} ${TARGET} *.xml *~
