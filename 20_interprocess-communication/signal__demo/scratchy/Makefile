SOURCE = scratchy.c ../lotharlib/console_input.c
OBJECTS = $(SOURCE:.c=.o)
TARGET = scratchy.elf
INC += -I../lotharlib/

.PREFIXES = .c .o
CC = gcc
## NB: -std=c99 and "struct sigaction won't compile,
##    since "c99" implicitely invokes: __STRICT_ANSI__
#CFLAGS = -g -Wall -Werror -Werror -std=c99 -pedantic

## ..thus use -std=gnu99
CFLAGS = -g -Wall -Werror -std=gnu99 -pedantic
RM = rm -f

%.c%.o:
	$(CC) $(INC) -c $(CFLAGS) $<


all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(INC) $(CFLAGS) -o $@ $(OBJECTS)

clean:
	$(RM) $(OBJECTS) $(TARGET) *~

.PHONY: all $(TARGET) clean

