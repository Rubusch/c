SOURCE = hello.asm
OBJECT = $(SOURCE:%.asm=%.o)
TARGET = $(SOURCE:%.asm=%.bin)

CC = nasm
LD = ld
RM = rm -f

%.o: %.asm
	$(CC) -g -f elf64 -o $(OBJECT) $<

all: $(OBJECT)
	$(LD) -o $(TARGET) $(OBJECT)
	## now execute the program via:
	## ./$(TARGET)

clean:
	$(RM) $(TARGET) $(OBJECT) *~

.PHONY: clean all
