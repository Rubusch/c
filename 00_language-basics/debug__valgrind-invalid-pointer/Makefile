SOURCE = Valgrind_invalid_pointer.c
OBJECTS = ${SOURCE:.c=.o}
.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -Werror -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} ${CFLAGS} -c $<

Valgrind_invalid_pointer.elf: ${OBJECTS}
	${CC} ${CFLAGS} -o $@ ${OBJECTS}

all: Valgrind_invalid_pointer.elf

clean:
	${RM} Valgrind_invalid_pointer.elf *~ *.o


