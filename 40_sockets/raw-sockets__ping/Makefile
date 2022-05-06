SOURCE = ping.c
HEADERS = ping.h
OBJECTS = $(SOURCE:.c=.o)
TARGET = $(SOURCE:.c=.elf)

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall # -Werror -std=c11 -pedantic
RM = rm -f

%.c%.o:
	$(CC) $(CFLAGS) -o $@ -c $<

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

## memory checks (address sanitizer)
##
## compile as follows
## $ unset LD_PRELOAD && make memcheck.elf
##
## execute (NB icmp sending needs privileges)
## $ sudo su
## #  export LD_PRELOAD=/usr/lib/arm-linux-gnueabihf/libasan.so.6 && ./memcheck.elf 127.0.0.1
##     PING 127.0.0.1 (127.0.0.1): 56 data bytes
##     64 bytes from 127.0.0.1: seq=1, ttl=64, rtt=0.460 ms
##     64 bytes from 127.0.0.1: seq=2, ttl=64, rtt=0.265 ms
## ^C
## # exit
##
## $ export LD_PRELOAD=libasan.so.6 && sudo ./memcheck.elf 127.0.0.1
##
## cleanup
## $ unset LD_PRELOAD && make clean
memcheck.elf: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ -fsanitize=address -fno-omit-frame-pointer ping.h $(OBJECTS)

clean:
	$(RM) $(OBJECTS) $(TARGET) *~ memcheck.elf

.PHONY: all clean
