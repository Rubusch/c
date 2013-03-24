# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = signalhandler01.c
OBJECTS = $(SOURCE:.c=.o)

CC = gcc
CFLAGS += -g -Wall
RM = rm -f

%.o: %.c
	$(CC) -c $(CFLAGS) $<

signalhandler01.exe: $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) -lm -pthread

.PHONY: all
all: signalhandler01.exe

.PHONY: clean
clean: 
	$(RM) $(OBJECTS) signalhandler01.exe *~


