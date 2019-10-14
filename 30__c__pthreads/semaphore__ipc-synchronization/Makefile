# Makefile (simple)
#
# use -O, -O1, -O2 for optimization

SOURCE = pthreads_ipc.c
TARGET = $(TARGET:.c=.exe)
OBJECTS = $(SOURCE:.c=.o)
CC = gcc
CFLAGS += -g -Wall
RM = rm -f

%.o: %.c
	$(CC) $(CFLAGS) -c $<

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) -lm -pthread

all: $(TARGET)

clean:
	$(RM) $(OBJECTS) $(TARGET) *~


