SOURCE = get_num.c
HEADERS_TEST = -I. -Iunittest
TARGET_TEST = test.elf
TARGET_BASIC_TEST = basictest.elf
TARGET_BASIC_TEST_AND_MEMORY = basictest_and_memory.elf

OBJECTS = $(SOURCE:.c=.o)

.PREFIXES = .c .o
CC ?= gcc
CFLAGS += -g -Wall
LDFLAGS += -lcunit
RM = rm -f

.c.o:
	$(CC) -c $(CFLAGS) $<

$(TARGET_TEST): $(OBJECTS)
	$(CC) $(CFLAGS) -DTESTING -o $@ unittest/test.c ${HEADERS_TEST} $(OBJECTS) $(LDFLAGS)

all: $(TARGET_TEST)

## for automation
## compile as follows
## $ make clean && make basictest.elf
$(TARGET_BASIC_TEST): $(OBJECTS)
	$(CC) $(CFLAGS) -DTESTING -DBASICTEST -o $@ unittest/test.c ${HEADERS_TEST} $(OBJECTS) $(LDFLAGS)

## memory checks (address sanitizer)
##
## to be started as follows (or similar):
## $ export LD_PRELOAD=libasan.so.5 && ./basictest_and_memory.elf
##
## compile then as follows
## $ unset LD_PRELOAD && make clean && make basictest_and_memory.elf
$(TARGET_BASIC_TEST_AND_MEMORY): $(OBJECTS)
	$(CC) $(CFLAGS) -DTESTING -DBASICTEST -o $@ unittest/test.c ${HEADERS_TEST} $(OBJECTS) $(LDFLAGS) -fsanitize=address -fno-omit-frame-pointer

clean:
	$(RM) $(OBJECTS) *.elf *.dot *.png *.xml ./unittest/*.xml *~ ./unittest/*~

.PHONY: clean all $(TARGET_TEST) $(TARGET_BASIC_TEST) $(TARGET_BASIC_TEST_AND_MEMORY)
