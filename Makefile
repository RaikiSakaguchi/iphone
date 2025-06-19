CC = gcc

BINDIR = ./bin
SRCDIR = ./src
INCDIR = ./include
# LIBDIR = ./lib

CFLAGS = -Wall -I$(INCDIR)
LDFLAGS = -L$(LIBDIR) -lm

SRCS = $(wildcard $(SRCDIR)/*.c)


