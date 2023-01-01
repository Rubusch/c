SOURCE = Valgrind_uninitialized_var.c
OBJECTS = ${SOURCE:.c=.o}
.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -std=c99 -pedantic #-Werror
RM = rm -f

.c.o:
	${CC} ${CFLAGS} -c $<

Valgrind_uninitialized_var.elf: ${OBJECTS}
	${CC} ${CFLAGS} -o $@ ${OBJECTS}

all: Valgrind_uninitialized_var.elf

clean:
	${RM} Valgrind_uninitialized_var.elf *~ *.o


