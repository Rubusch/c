SOURCE = main.c foo.c
HEADERS = foo.h
OBJECTS = $(SOURCE:.c=.o)
VERSION = 1.0
TARGET = demo-$(VERSION).elf

.PREFIXES = .c .o
CC = gcc
## link-time-optimization (GCC 10) - providing context to the linker, e.g. VERSION
#COMMON_LDFLAGS = -flto
#COMMON_CFLAGS = -flto
## NB: available after gcc v10, before we will have the following error
## $ make clean && make
## rm -f demo-1.0.elf main.o foo.o *~ *.elf
## gcc -flto -g -Wall -Werror -std=c11 -pedantic   -c -o main.o main.c
## gcc -flto -g -Wall -Werror -std=c11 -pedantic   -c -o foo.o foo.c
## gcc -flto -g -Wall -Werror -std=c11 -pedantic -o demo-1.0.elf main.o foo.o -lm
## /usr/bin/ld: /usr/bin/ld: DWARF error: invalid abstract instance DIE ref
## /tmp/ccPD5HHa.ltrans0.ltrans.o: in function `main':
## <artificial>:(.text+0xa4): undefined reference to `public_function'
## collect2: error: ld returned 1 exit status
## make: *** [Makefile:25: demo-1.0.elf] Error 1

CFLAGS = $(COMMON_CFLAGS) -g -Wall -Werror -std=c11 -pedantic
LDFLAGS = $(COMMON_LDFLAGS) -Wl,--version-script,version.lds -version-number `echo $(VERSION) | sed 's/\./:/g'`
RM = rm -f

%.c%.o:
	$(CC) $(CFLAGS) -c $<

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

clean:
	$(RM) $(TARGET) $(OBJECTS) *~ *.elf

.PHONY: all clean
