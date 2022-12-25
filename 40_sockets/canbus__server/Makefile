TARGET = can_beacon.elf
SOURCE = can_interface.c can_frame.c

HEADERS_TEST = -I. -Iunittest
TARGET_TEST = test.elf
TARGET_BASIC_TEST = basictest.elf
TARGET_BASIC_TEST_AND_MEMORY = basictest_and_memory.elf

OBJECTS = $(SOURCE:.c=.o)
.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -std=gnu99 -pedantic -Werror
LDFLAGS += -lcunit
RM = rm -f

.c.o:
	$(CC) $(CFLAGS) -c $<

#TAGS:
#	$(shell find . -name "*.c" -exec etags -a {} \; )
#	$(shell find . -name "*.h" -exec etags -a {} \; )

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -c $<
	$(CC) $(CFLAGS) -o $@ main.c $(OBJECTS) -pthread

all: $(TARGET)


$(TARGET_TEST):
	$(CC) $(CFLAGS) -DTESTING -o $@ unittest/test.c ${HEADERS_TEST} $(SOURCE) $(LDFLAGS) -pthread

## basictest
$(TARGET_BASIC_TEST):
	$(CC) $(CFLAGS) -DTESTING -DBASICTEST -o $@ unittest/test.c ${HEADERS_TEST} $(SOURCE) $(LDFLAGS) -pthread

## memory checks (address sanitizer)
##
## to be started as follows (or similar):
## $ export LD_PRELOAD=libasan.so.5 && ./basictest_and_memory.elf
##
## compile then as follows
## $ unset LD_PRELOAD && make basictest_and_memory.elf
$(TARGET_BASIC_TEST_AND_MEMORY):
	$(CC) $(CFLAGS) -DTESTING -DBASICTEST -o $@ unittest/test.c ${HEADERS_TEST} $(SOURCE) $(LDFLAGS)  -fsanitize=address -fno-omit-frame-pointer -pthread



clean:
	$(RM) $(OBJECTS) $(TARGET) *~ *.log core *.elf *.o *.xml ./unittest/*~

.PHONY: clean all $(TARGET_TEST) $(TARGET_BASIC_TEST) $(TARGET_BASIC_TEST_AND_MEMORY)
