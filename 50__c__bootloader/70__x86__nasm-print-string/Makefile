SOURCE = boot.nasm
TARGET = $(SOURCE:%.nasm=%.bin)

CC = nasm
LD = ld
RM = rm -f

%.bin: %.nasm
	$(CC) -o $(TARGET) -f bin $<

all: $(TARGET)
	## now test it in qemu:
	## qemu-system-x86_64 -drive format=raw,file=boot.bin

clean:
	$(RM) $(TARGET) *~

.PHONY: clean all
