# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = timemeasuring.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

timemeasuring: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm -lrt
# linked against librt for the time.h functions -> -lrt

all: timemeasuring

clean: 
	${RM} $(OBJECTS) timemeasuring *~ core


