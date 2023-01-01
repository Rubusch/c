SOURCE = utility.c

SOURCEMAIN = main.c
TARGET = $(SOURCEMAIN:.c=.elf)

SOURCETEST = test__utility.c
TARGETTEST = $(SOURCETEST:.c=.elf)

OBJECTS = $(SOURCE:.c=.o)

.PREFIXES = .c .o
CC ?= gcc
CFLAGS += -g -Wall -Werror
LDFLAGS += -lcunit
RM = rm -f

all:	$(TARGET) $(TARGETTEST)

$(TARGETTEST): test__utility.c $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ test__utility.c $(OBJECTS) $(LDFLAGS)

$(TARGET): $(SOURCEMAIN) $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(SOURCEMAIN) $(OBJECTS)

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	$(RM) $(TARGET) $(TARGETTEST) *.o *.xml *~
