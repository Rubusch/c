# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = kirk.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall 
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

kirk: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm -D _SVID_SOURCE=1 -D _XOPEN_SOURCE=600

all: kirk

clean: 
	${RM} $(OBJECTS) kirk *~ core


