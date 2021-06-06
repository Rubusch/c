SOURCE = hello.asm
OBJECT = $(SOURCE:%.asm=%.o)
TARGET = $(SOURCE:%.asm=%.bin)

CC = nasm
LD = ld
RM = rm -f

%.o: %.asm
	$(CC) -f elf64 -o $(OBJECT) $<

all: $(OBJECT)
	$(LD) -o $(TARGET) $(OBJECT)

clean:
	$(RM) $(TARGET) $(OBJECT) *~

.PHONY: clean all
