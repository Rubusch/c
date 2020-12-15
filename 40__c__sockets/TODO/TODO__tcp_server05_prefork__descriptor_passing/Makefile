# Makefile (simple)
#
# use -O, -O1, -O2 for optimization

SOURCE = tcp_server05.c setup_signal.c work_server.c readn.c writen.c read_fd.c write_fd.c
OBJECTS = ${SOURCE:.c=.o}
HEADERS =

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall
RM = rm -f

.c.o:
	${CC} -o $@ -c ${CFLAGS} $<

tcp_server05.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS}

all: tcp_server05.exe

clean:
	${RM} $(OBJECTS) tcp_server05.exe *~ core
