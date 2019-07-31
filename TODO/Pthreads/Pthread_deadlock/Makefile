# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = deadlock.c
OBJECTS = ${SOURCE:.c=.o}

CC = gcc
CFLAGS += -g -Wall
RM = rm -f

%.o: %.c
	${CC} -c ${CFLAGS} $<

deadlock.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm -pthread

.PHONY: all
all: deadlock.exe

.PHONY: clean
clean: 
	${RM} $(OBJECTS) deadlock.exe *~


