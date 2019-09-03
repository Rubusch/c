## C Makefile for linux
##
## takes all in BUILDDIR with EXTension .c to be compiled to .o
##
## resource: http://c-plusplus.de/forum/viewtopic-var-t-is-88418.html
##
## @author: Lothar Rubusch

TARGET   := NAME.exe
CLAGS += -g -Wall -Werror -std=c99 -pedantic
CXX      := gcc
LIBS     +=
EXT      := c
BUILDDIR := build

override BUILDDIR := $(strip $(BUILDDIR))
SOURCES  := $(wildcard *.$(EXT))
OBJECTS  := $(patsubst %.$(EXT), $(BUILDDIR)/%.o, $(SOURCES))
DEPS     := $(patsubst %.$(EXT), $(BUILDDIR)/%.dep, $(SOURCES))


all: $(TARGET)

$(TARGET): $(OBJECTS) $(DEPS)
	$(CXX) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)

ifneq ($(MAKECMDGOALS), clean)
-include $(DEPS)
endif

$(OBJECTS): $(BUILDDIR)/%.o: %.$(EXT) $(BUILDDIR)/%.dep $(BUILDDIR)/.tag
	$(CXX) $(CFLAGS) -c $< -o $@

$(DEPS): $(BUILDDIR)/%.dep: %.$(EXT) $(BUILDDIR)/.tag
	mkdir -p $(dir $(@))
	$(CXX) $(CFLAGS) -MM $< -MT $@ -MT $(<:.$(EXT)=.o) -o $@

%.tag:
	mkdir -p $(dir $(@))
	touch $@

clean:
	rm -rf $(BUILDDIR)

.PHONY: clean all
