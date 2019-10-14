# Makefile (simple)
#
# use -O, -O1, -O2 for optimization
#
# -std=c99 -pedantic can cause problems with PTHREADS


SOURCE = prod_cons.c
TARGET = $(SOURCE:.c=.exe)
OBJECTS = $(SOURCE:.c=.o)
CC = gcc
CFLAGS += -g -Wall -pedantic -Werror
RM = rm -f

%.o: %.c
	$(CC) $(CFLAGS) -c $<

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) -lm -pthread

all: $(TARGET)

clean:
	$(RM) $(OBJECTS) $(TARGET) *~


