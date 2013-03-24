## Makefile
##
## 

SOURCE  = segviolation.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc 
CFLAGS = -g -Wall -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

segviolation: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: segviolation

clean:
	${RM}  ${OBJECTS} segviolation *~ core
