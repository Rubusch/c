## Makefile (simple)
##
## @author: Lothar Rubusch
## @email: L.Rubusch@gmx.ch
## @license: GPLv3
##
## use -O, -O1, -O2 for optimization


SOURCE = main.c tree_access.c tree/tree.c lotharlib/console_input.c
HEADERS = tree_access.h tree/tree.h lotharlib/console_input.h lotharlib/mathtools.h
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall
RM = rm -f

.c.o:
	${CC} -o $@ -c ${CFLAGS} $<

treetest.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: treetest.exe

clean:
	${RM} $(OBJECTS) treetest.exe *~ core


