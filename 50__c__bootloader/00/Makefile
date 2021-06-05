SOURCE = boot.s
OBJECT = $(SOURCE:%.s=%.o)
TARGET = $(SOURCE:%.s=%.exe)

CC = as
RM = rm -f

%.o: %.s
	$(CC) -o $(OBJECT) $<

all: $(OBJECT)

clean:
	$(RM) $(TARGET) $(OBJECT) *~

.PHONY: clean all
