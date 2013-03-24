# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 

SOURCE = pthread_join.c
OBJECTS = $(SOURCE:.c=.o)

CC = gcc
CFLAGS += -g -Wall -std=c99 -pedantic 
RM = rm -f

%.o: %.c
	$(CC) -c $(CFLAGS) $<

pthread_join.exe: $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) -lm -pthread

.PHONY: all
all: pthread_join.exe

.PHONY: clean
clean: 
	$(RM) $(OBJECTS) pthread_join.exe *~ 


