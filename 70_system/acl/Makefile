SOURCE = acl.c
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
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) -lacl

all: $(TARGET)

clean:
	-$(RM) $(TARGET) $(OBJECTS) text.txt *~

.PHONY: clean all
