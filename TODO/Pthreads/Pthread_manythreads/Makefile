# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 
#
# -std=c99 -pedantic can cause problems with PTHREADS


SOURCE = manythreads.c
OBJECTS = $(SOURCE:.c=.o)

CC = gcc
CFLAGS += -g -Wall
RM = rm -f

%.o: %.c
	$(CC) -c $(CFLAGS) $<

manythreads.exe: $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) -lm -pthread

.PHONY: all
all: manythreads.exe

.PHONY: clean
clean: 
	$(RM) $(OBJECTS) manythreads.exe *~


