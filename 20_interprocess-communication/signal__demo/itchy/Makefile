SOURCE = itchy.c
OBJECTS = $(SOURCE:.c=.o)
TARGET = $(SOURCE:.c=.elf)

.PREFIXES = .c .o
CC = gcc
## NB: -std=c99 and "struct sigaction won't compile,
##    since "c99" implicitely invokes: __STRICT_ANSI__
#CFLAGS = -g -Wall -Werror -Werror -std=c99 -pedantic

## ..thus use -std=gnu99
CFLAGS = -g -Wall -std=gnu99 -pedantic
RM = rm -f

%.c%.o:
	$(CC) -c $(CFLAGS) $<


all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)


clean:
	$(RM) $(OBJECTS) $(TARGET) *~

.PHONY: all $(TARGET) clean

