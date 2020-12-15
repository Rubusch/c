# Makefile (simple)
#
# use -O, -O1, -O2 for optimization

SOURCE = tcp_server04.c setup_signal.c work_server.c readn.c writen.c lock_pthread.c
OBJECTS = ${SOURCE:.c=.o}
HEADERS =

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall
RM = rm -f

.c.o:
	${CC} -o $@ -c ${CFLAGS} $<

tcp_server04.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm -pthread

all: tcp_server04.exe

clean:
	${RM} $(OBJECTS) tcp_server04.exe *~ core
