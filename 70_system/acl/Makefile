SOURCE = acl.c
HEADERS = file_perms.h
OBJECTS = $(SOURCE:.c=.o)
TARGET = $(SOURCE:.c=.elf)
.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -Werror -std=gnu99 -pedantic
RM = rm -f

.c.o:
	$(CC) $(CFLAGS) -c $<

$(TARGET): $(OBJECTS)
	cp -f template.txt text.txt
	## NB: needs -lacl LINKER flag
	$(CC) $(CFLAGS) file_perms.c -o $@ $(OBJECTS) -lacl

all: $(TARGET)

clean:
	-$(RM) $(TARGET) $(OBJECTS) file_perms.o text.txt *~

.PHONY: clean all
