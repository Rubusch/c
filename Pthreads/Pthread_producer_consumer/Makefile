# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 
#
# -std=c99 -pedantic can cause problems with PTHREADS


SOURCE = prod_cons.c
OBJECTS = $(SOURCE:.c=.o)

CC = gcc
CFLAGS += -g -Wall
RM = rm -f

%.o: %.c
	$(CC) -c $(CFLAGS) $<

prod_cons.exe: $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) -lm -pthread

.PHONY: all
all: prod_cons.exe

.PHONY: clean
clean: 
	$(RM) $(OBJECTS) prod_cons.exe *~


