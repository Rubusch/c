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

clean:
	$(RM) $(TARGET) $(OBJECT) *~

.PHONY: clean all
