SOURCE = gpio.c
TARGET = gpio.elf
OBJECTS = $(SOURCE:.c=.o)
LIBS += -lgpiod
.PREFIXES = .c .o
RM = rm -f
## NB: don't set CC when using yocto SDK (overwrites)
## NB: similar to CFLAGS

.c.o:
	$(CC) $(LIBS) $(CFLAGS) -c $<


$(TARGET): $(OBJECTS)
	$(CC) $(LIBS) $(CFLAGS) -o $@ $(OBJECTS)


all:    $(TARGET)

clean:
	$(RM) $(OBJECTS) $(TARGET) *~

.PHONY: all clean

