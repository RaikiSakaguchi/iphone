CC = gcc

BINDIR = ./bin
SRCDIR = ./src
INCDIR = ./include
OBJDIR = ./obj
# LIBDIR = ./lib

CFLAGS = -Wall -O2 -I$(INCDIR)
LDFLAGS = 

SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))
TARGET = $(BINDIR)/iphone

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS) | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

clean:
	rm -rf $(OBJDIR) $(BINDIR)


