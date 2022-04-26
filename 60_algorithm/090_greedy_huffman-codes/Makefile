SOURCE = greedy-huffman.c
HEADERS_TEST = -I. -Iunittest
TARGET_TEST = test.exe
TARGET_BASIC_TEST = basictest.exe
TARGET_BASIC_TEST_AND_MEMORY = basictest_and_memory.exe

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

## memory checks (address sanitizer)
##
## compile then as follows
## $ unset LD_PRELOAD && make clean && make basictest_and_memory.exe
##
## to be started as follows (or similar):
## $ export LD_PRELOAD=libasan.so.6 && ./basictest_and_memory.exe
$(TARGET_BASIC_TEST_AND_MEMORY): $(OBJECTS)
	$(CC) $(CFLAGS) -DBASICTEST -o $@ unittest/test.c ${HEADERS_TEST} $(OBJECTS) $(LDFLAGS) -fsanitize=address -fno-omit-frame-pointer

clean:
	$(RM) $(OBJECTS) *.exe *.dot *.png *.xml ./unittest/*.xml *~ ./unittest/*~

.PHONY: clean all $(TARGET_TEST) $(TARGET_BASIC_TEST) $(TARGET_BASIC_TEST_AND_MEMORY)
