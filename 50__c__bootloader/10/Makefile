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
	## provide format: raw (binary), no ELF overhead
	## set entry point: 'init'

clean:
	$(RM) $(TARGET) $(OBJECT) *~

.PHONY: clean all
