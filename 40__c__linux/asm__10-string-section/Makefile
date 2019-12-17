## Makefile
##
## @author: Lothar Rubusch
## @email: L.Rubusch@gmx.ch
## @license: GPLv3 2013
##
## use -O, -O1, -O2 for optimization

SOURCE = rabbit.c
OBJECTS = $(SOURCE:%.c=%.o)
TARGET = $(SOURCE:%.c=%.exe)
CC = gcc
## NOTE: rabbit.o: relocation R_X86_64_32S against `.text' can not be used when making a PIE object; recompile with -fPIC
## NOTE: -pedantic: a member of a structure or union cannot have a variably modified type (used to convert long into char array)
CFLAGS += -g -Wall -fPIC -no-pie -std=gnu99 -pedantic
RM = rm -f

%.o: %.c
	$(CC) $(CFLAGS) -c $<

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

clean:
	$(RM) $(OBJECTS) $(TARGET) *~

.PHONY: clean all
