# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = shmclient.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

shmclient: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm -D _SVID_SOURCE=1 -D _XOPEN_SOURCE=600

all: shmclient

clean: 
	${RM} $(OBJECTS) shmclient *~ core


