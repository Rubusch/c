SOURCE = boot.s
OBJECT = $(SOURCE:%.s=%.o)
TARGET = $(SOURCE:%.s=%.bin)

CC = as
LD = ld
RM = rm -f

%.o: %.s
	$(CC) -o $(OBJECT) $<

all: $(OBJECT)
	$(LD) -o $(TARGET) --oformat binary -e init $(OBJECT)
	## now test it in qemu:
	## $ qemu-system-x86_64 -drive format=raw,file=boot.bin

clean:
	$(RM) $(TARGET) $(OBJECT) *~

.PHONY: clean all
