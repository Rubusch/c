SRC_HANDLER = sighandler.c
OBJ_HANDLER = sighandler.o
SRC_QUEUE = sigqueue.c
OBJ_QUEUE = sigqueue.o

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -Werror -std=gnu99 -pedantic
RM = rm -f

all: sighandler.elf sigqueue.elf

sighandler.elf: $(SRC_HANDLER)
	$(CC) $(CFLAGS) -c $<
	$(CC) $(CFLAGS) tlpi/get_num.c -o $@ $(OBJ_HANDLER)

sigqueue.elf: $(SRC_QUEUE)
	$(CC) $(CFLAGS) -c $<
	$(CC) $(CFLAGS) tlpi/get_num.c -o $@ $(OBJ_QUEUE)

clean:
	-$(RM) *.elf $(OBJ_QUEUE) $(OBJ_HANDLER) *~

.PHONY: clean all
