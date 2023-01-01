SOURCE = Valgrind_mem_leak.c
OBJECTS = ${SOURCE:.c=.o}
.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -Werror -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} ${CFLAGS} -c $<

Valgrind_mem_leak.elf: ${OBJECTS}
	${CC} ${CFLAGS} -o $@ ${OBJECTS}

all: Valgrind_mem_leak.elf

clean:
	${RM} Valgrind_mem_leak.elf *~ *.o


