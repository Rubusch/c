## Makefile (simple)
##
## @author: Lothar Rubusch <L.Rubusch@gmail.com>
## @license: GPLv3
TARGET = can_beacon.exe
SOURCE = can_interface.c can_frame.c

HEADERS_TEST = -I. -Iunittest
TARGET_TEST = test.exe
TARGET_BASIC_TEST = basictest.exe

OBJECTS = $(SOURCE:.c=.o)
.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -std=gnu99 -pedantic -Werror
LDFLAGS += -lcunit
RM = rm -f

.c.o:
	$(shell find . -name "*.c" -exec etags -a {} \; )
	$(shell find . -name "*.h" -exec etags -a {} \; )
	$(CC) $(CFLAGS) -c $<

$(TARGET): $(OBJECTS) TAGS
	$(CC) $(CFLAGS) -o $@ main.c $(OBJECTS) -pthread

all: $(TARGET)


$(TARGET_TEST): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ unittest/test.c ${HEADERS_TEST} $(OBJECTS) $(LDFLAGS) -pthread

## for automation
$(TARGET_BASIC_TEST): $(OBJECTS)
	$(CC) $(CFLAGS) -DBASICTEST -o $@ unittest/test.c ${HEADERS_TEST} $(OBJECTS) $(LDFLAGS)  -pthread


clean:
	$(RM) $(OBJECTS) $(TARGET) TAGS *~ *.log core

.PHONY: clean all $(TARGET_TEST) $(TARGET_BASIC_TEST)
