#
# Kelechi Igwe (kigwe) and Austin Ng (ang24)
#
# This is the Makefile for Lab 3A.
#

CC = gcc
CFLAGS = -Wall -pedantic
LD = gcc
LDFLAGS =

MAIN = intsort
SRCS = main.c sort.c

all: $(MAIN)

clean:
	rm -f $(MAIN)

$(MAIN): $(SRCS)
	@$(CC) $(CFLAGS) -g -o $(MAIN) $(SRCS) -pthread
