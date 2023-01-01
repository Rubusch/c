SOURCE = test.c
OBJECTS = ${SOURCE:.c=.o}
.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -Werror -pedantic -std=c99
RM = rm -f

.c.o:
	${CC} ${CFLAGS} -c $<

test.elf: ${OBJECTS}
	${CC} ${CFLAGS} -o $@ ${OBJECTS}

clean:
	${RM} test.elf ${OBJECTS} *~


