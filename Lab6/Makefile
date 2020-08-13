#
# Kelechi Igwe (kigwe), Austin Ng (ang24)
#
# This is the Makefile for Lab 6, "Contiguous Memory Allocation".
#

CC = gcc
CFLAGS = -Wall
LD = gcc
LDFLAGS =

MAIN = allocator
SRCS = main.c allocator.c

all: $(MAIN)

clean:
	rm -f $(MAIN)

$(MAIN): $(SRCS)
	@$(CC) $(CFLAGS) -g -o $(MAIN) $(SRCS)
