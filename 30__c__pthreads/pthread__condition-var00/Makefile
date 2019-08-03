# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = conditionalvariable.c
OBJECTS = $(SOURCE:.c=.o)

CC = gcc
CFLAGS += -g -Wall -std=c99 -pedantic
RM = rm -f

%.o: %.c
	$(CC) -c $(CFLAGS) $<

conditionalvariable.exe: $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) -lm -pthread

.PHONY: all
all: conditionalvariable.exe

.PHONY: clean
clean: 
	$(RM) $(OBJECTS) conditionalvariable.exe *~


