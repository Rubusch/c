SOURCE = fflush.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -Werror -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} ${CFLAGS} -c $<

fflush.elf: ${OBJECTS}
	${CC} ${CFLAGS} -o $@ ${OBJECTS}

all: fflush.elf

clean:
	${RM} $(OBJECTS) fflush.elf *~
