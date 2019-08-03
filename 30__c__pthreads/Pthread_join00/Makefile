# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = join.c
OBJECTS = $(SOURCE:.c=.o)

CC = gcc
CFLAGS += -g -Wall
RM = rm -f

%.o: %.c
	$(CC) -c $(CFLAGS) $<

join.exe: $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) -lm -pthread

.PHONY: all
all: join.exe

.PHONY: clean
clean: 
	$(RM) $(OBJECTS) join.exe *~


