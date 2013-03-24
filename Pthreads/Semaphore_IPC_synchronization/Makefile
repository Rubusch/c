# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = pthreads_ipc.c
OBJECTS = $(SOURCE:.c=.o)

CC = gcc
CFLAGS += -g -Wall
RM = rm -f

%.o: %.c
	$(CC) -c $(CFLAGS) $<

pthreads_ipc.exe: $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) -lm -pthread

.PHONY: all
all: pthreads_ipc.exe

.PHONY: clean
clean: 
	$(RM) $(OBJECTS) pthreads_ipc.exe *~


