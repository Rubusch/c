SOURCE = insertionsort.c
HEADERS = insertionsort.h
TARGET_TEST = test_insertionsort.exe

OBJECTS = $(SOURCE:.c=.o)

.PREFIXES = .c .o
CC ?= gcc
CFLAGS += -g -Wall
LDFLAGS += -lcunit
RM = rm -f

.c.o:
	$(CC) -c $(CFLAGS) $<

$(TARGET_TEST): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ test_insertionsort.c $(OBJECTS) $(LDFLAGS)

all: $(TARGET_TEST)

.PHONY: clean all $(TARGET_TEST)

clean:
	$(RM) $(OBJECTS) $(TARGET_TEST) *~


