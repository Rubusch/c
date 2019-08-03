# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = pthread_stack_management.c
OBJECTS = ${SOURCE:.c=.o}

CC = gcc
CFLAGS += -g -Wall -std=c99 -pedantic 
RM = rm -f

%.o: %.c
	${CC} -c ${CFLAGS} $<

pthread_stack_management.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm -pthread

.PHONY: all
all: pthread_stack_management.exe

.PHONY: clean
clean: 
	${RM} $(OBJECTS) pthread_stack_management.exe *~


