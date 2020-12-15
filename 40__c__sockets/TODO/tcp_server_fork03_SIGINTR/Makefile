# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = tcp_server_fork03.c
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

tcp_server_fork03.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS}
#	${CC} -o $@ ${OBJECTS} -lm -pthread

all: tcp_server_fork03.exe

clean: 
	${RM} $(OBJECTS) tcp_server_fork03.exe *~ core


