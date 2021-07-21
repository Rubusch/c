# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = main.c tree_control.c tree_backend.c tools.c tree/tree.c
HEADERS = tree_control.h tree_backend.h tools.h lotharlib/mathtools.h tree/tree.h 
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall
RM = rm -f

.c.o:
	${CC} -o $@ -c ${CFLAGS} $<

treetest: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: treetest

clean: 
	${RM} $(OBJECTS) treetest *~ core


