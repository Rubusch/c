SOURCEFILE = canbus.c
SOURCE = ./canbus__01/$(SOURCEFILE)

TARGET = $(SOURCEFILE:.c=.elf)
OBJECTS = $(SOURCEFILE:.c=.o)

SOURCETEST = test__canbus.c
TARGETTEST = $(SOURCETEST:.c=.elf)

.PREFIXES = .c .o
CC ?= gcc
CFLAGS ?= -g -Wall -Wno-unused-but-set-variable -Wno-implicit-function-declaration -Wno-unused-variable
LDFLAGS ?= -lcunit
RM = rm -f

all: $(TARGET) $(TARGETTEST)

$(TARGETTEST): $(SOURCETEST) $(SOURCE)
	$(CC) -I./ $(CFLAGS) -DTESTING -o $@ $(SOURCETEST) $(SOURCE) $(LDFLAGS)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $@ $(SOURCE)

clean:
	$(RM) $(OBJECTS) $(TARGET) $(TARGETTEST) *~ ./canbus__01/*~
