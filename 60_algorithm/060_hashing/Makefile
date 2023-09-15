SOURCE = hashtable.c
HEADERS_TEST = -I. -Iunittest
TARGET_TEST = test.elf
TARGET_BASIC_TEST = basictest.elf

OBJECTS = $(SOURCE:.c=.o)

.PREFIXES = .c .o
CC ?= gcc
CFLAGS += -g -Wall
LDFLAGS += -lcunit
RM = rm -f

.c.o:
	$(CC) -c $(CFLAGS) $<

$(TARGET_TEST): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ unittest/test.c ${HEADERS_TEST} $(OBJECTS) $(LDFLAGS)

all: $(TARGET_TEST)

## for automation
$(TARGET_BASIC_TEST): $(OBJECTS)
	$(CC) $(CFLAGS) -DBASICTEST -o $@ unittest/test.c ${HEADERS_TEST} $(OBJECTS) $(LDFLAGS)

.PHONY: clean all $(TARGET_TEST) $(TARGET_BASIC_TEST)

clean:
	$(RM) $(OBJECTS) $(TARGET_TEST) $(TARGET_BASIC_TEST) *.dot *.png *.xml ./unittest/*.xml *~ ./unittest/*~
