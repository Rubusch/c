# Makefile (simple)
#
# use -O, -O1, -O2 for optimization

SOURCE = tcp_server03.c setup_signal.c work_server.c readn.c writen.c lock_fcntl.c
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

tcp_server03.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS}

all: tcp_server03.exe

clean:
	${RM} $(OBJECTS) tcp_server03.exe *~ core
