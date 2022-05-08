SOURCE = rabbit.c
OBJECTS = $(SOURCE:%.c=%.o)
TARGET = $(SOURCE:%.c=%.elf)
CC = gcc
## NOTE: rabbit.o: relocation R_X86_64_32S against `.text' can not be used when making a PIE object; recompile with -fPIC - check ".forward:" with dot, check "\\0" at end of string
## NOTE: -pedantic: a member of a structure or union cannot have a variably modified type (used to convert long into char array)
CFLAGS += -g -Wall -std=gnu99 -pedantic
RM = rm -f

%.o: %.c
	$(CC) $(CFLAGS) -c $<

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

clean:
	$(RM) $(OBJECTS) $(TARGET) *~

.PHONY: clean all
