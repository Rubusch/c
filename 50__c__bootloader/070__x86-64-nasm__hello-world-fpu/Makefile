ASM_SOURCE = hello.asm
ASM_OBJECT = $(ASM_SOURCE:%.asm=%.o)

SOURCE = print.c
OBJECT = $(SOURCE:%.c=%.o)

OBJECTS = $(ASM_OBJECT) $(OBJECT)
TARGET = $(ASM_SOURCE:%.asm=%.bin)
LIB = /lib64/ld-linux-x86-64.so.2

CC = gcc
CFLAGS = -g
AS = nasm
LD = ld
RM = rm -f

all: $(OBJECT) $(ASM_OBJECT)
	$(LD) -dynamic-linker $(LIB) -lc $(OBJECTS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $(SOURCE) -o $(OBJECT)

%.o: %.asm
	$(AS) -f elf64 $(ASM_SOURCE) -o $(ASM_OBJECT)


clean:
	$(RM) $(TARGET) $(OBJECTS) *~


.PHONY: clean all
