# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = pthread_passing_arguments.c
OBJECTS = $(SOURCE:.c=.o)

CC = gcc
CFLAGS += -g -Wall -std=c99 -pedantic 
RM = rm -f

%.o: %.c
	$(CC) -c $(CFLAGS) $<

pthread_passing_arguments.exe: $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) -lm -pthread

.PHONY: all
all: pthread_passing_arguments.exe

.PHONY: clean
clean: 
	$(RM) $(OBJECTS) pthread_passing_arguments.exe *~


