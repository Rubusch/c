# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = client.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

Client: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: Client

clean: 
	${RM} $(OBJECTS) Client *~ core


