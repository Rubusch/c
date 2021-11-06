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
	## now execute the program to sum two numbers:
	## ./$(TARGET) <number> <number>

clean:
	$(RM) $(TARGET) $(OBJECT) *~

.PHONY: clean all
