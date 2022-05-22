ACCT_SOURCE = accounting.c
ACCT = $(ACCT_SOURCE:.c=.elf)
ACCT_OBJECTS = $(ACCT_SOURCE:.c=.o)

ACCT_VIEWER_SOURCE = accounting_viewer.c
ACCT_VIEWER = $(ACCT_VIEWER_SOURCE:.c=.elf)
ACCT_VIEWER_OBJECTS = $(ACCT_VIEWER_SOURCE:.c=.o)

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -std=gnu99 -pedantic -Werror
RM = rm -f

all: $(ACCT) $(ACCT_VIEWER)

.c.o:
	$(CC) $(CFLAGS) -c $<

$(ACCT): $(ACCT_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(ACCT_OBJECTS)

$(ACCT_VIEWER): $(ACCT_VIEWER_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(ACCT_VIEWER_OBJECTS)



clean:
	$(RM) $(ACCT_OBJECTS) $(ACCT) $(ACCT_VIEWER_OBJECTS) $(ACCT_VIEWER) core *~

.PHONY: all clean $(ACCT) $(ACCT_VIEWER)

