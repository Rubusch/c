# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = select.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall
RM = rm -f

## without header:
.c.o:
	${CC} -c ${CFLAGS} $<

select.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} 

all: select.exe

clean: 
	${RM} $(OBJECTS) select.exe *~ core


