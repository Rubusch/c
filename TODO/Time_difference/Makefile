# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = difftime.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

difftime: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: difftime

clean: 
	${RM} $(OBJECTS) difftime *~ core


