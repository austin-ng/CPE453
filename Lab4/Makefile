#
# Kelechi Igwe (kigwe) and Austin Ng (ang24)
#
# This is the Makefile for Lab 4, "Readers/Writers Problem Implement...".
#

CC = gcc
CFLAGS = -Wall -pedantic
LD = gcc
LDFLAGS =

MAIN = rw_problem
SRCS = main.c rw_threads.c

all: $(MAIN)

clean:
	rm -f $(MAIN)

$(MAIN): $(SRCS)
	@$(CC) $(CFLAGS) -g -o $(MAIN) $(SRCS) -pthread
