# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = signalhandler02.c
OBJECTS = $(SOURCE:.c=.o)

CC = gcc
CFLAGS += -g -Wall
RM = rm -f

%.o: %.c
	$(CC) -c $(CFLAGS) $<

signalhandler02.exe: $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) -lm -pthread

.PHONY: all
all: signalhandler02.exe

.PHONY: clean
clean: 
	$(RM) $(OBJECTS) signalhandler02.exe *~


