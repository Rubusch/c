# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = signalhandler.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

signalhandler.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: signalhandler.exe

clean: 
	${RM} $(OBJECTS) signalhandler.exe *~ core


