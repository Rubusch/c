# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = cancellation.c
OBJECTS = $(SOURCE:.c=.o)

CC = gcc
CFLAGS += -g -Wall
RM = rm -f

%.o: %.c
	$(CC) -c $(CFLAGS) $<

cancellation.exe: $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) -lm -lpthread

.PHONY: all
all: cancellation.exe

.PHONY: clean
clean: 
	$(RM) $(OBJECTS) cancellation.exe *~

