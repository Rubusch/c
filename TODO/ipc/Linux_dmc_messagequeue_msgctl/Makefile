# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = msgctl.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

msgctl: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm -D _SVID_SOURCE=1 -D _XOPEN_SOURCE=600

all: msgctl

clean: 
	${RM} $(OBJECTS) msgctl *~ core


