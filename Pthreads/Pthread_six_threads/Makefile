# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = multithread.c
OBJECTS = $(SOURCE:.c=.o)

CC = gcc
CFLAGS += -g -Wall
RM = rm -f

%.o: %.c
	$(CC) -c $(CFLAGS) $<

multithread.exe: $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) -lm -lpthread

.PHONY: all
all: multithread.exe

.PHONY: clean
clean: 
	$(RM) $(OBJECTS) multithread.exe *~


