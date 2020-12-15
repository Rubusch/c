# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = tcp_client04.c
OBJECTS = ${SOURCE:.c=.o}
HEADERS = 

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall
RM = rm -f

.c.o:
	${CC} -o $@ -c ${CFLAGS} $<

## without header:
#.c.o:
#	${CC} -c ${CFLAGS} $<

tcp_client04.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS}

all: tcp_client04.exe

clean: 
	${RM} $(OBJECTS) tcp_client04.exe *~ core


