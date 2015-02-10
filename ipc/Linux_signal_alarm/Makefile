# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = alarm.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

alarm: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: alarm

clean: 
	${RM} $(OBJECTS) alarm *~ core


