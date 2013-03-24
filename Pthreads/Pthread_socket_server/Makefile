# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = sock_server.c
OBJECTS = $(SOURCE:.c=.o)

CC = gcc
CFLAGS += -g -Wall
RM = rm -f

%.o: %.c
	$(CC) -c $(CFLAGS) $<

sock_server.exe: $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) -lm -lpthread

all: sock_server.exe

clean: 
	$(RM) $(OBJECTS) sock_server.exe *~
