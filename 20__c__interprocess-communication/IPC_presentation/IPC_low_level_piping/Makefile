# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = lowlevelpipe.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

lowlevelpipe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: lowlevelpipe

clean: 
	${RM} $(OBJECTS) lowlevelpipe *~ core


