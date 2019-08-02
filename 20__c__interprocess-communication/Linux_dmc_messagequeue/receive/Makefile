# Makefile
#
# .h and .c files

SOURCE = message_rec.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

mq_receive: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm -D _SVID_SOURCE=1 -D _XOPEN_SOURCE=600

all:	mq_receive

clean:
	${RM} ${OBJECTS} mq_receive *~ core


