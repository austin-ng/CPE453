#
# Kelechi Igwe (kigwe) and Austin Ng (ang24)
#
# This is the Makefile for Lab 3B, "Implement Matrix Addition using pthreads".
#

CC = gcc
CFLAGS = -Wall -pedantic -fopenmp
LD = gcc
LDFLAGS =

MAIN = matrix_add
SRCS = main.c matrix_add.c

all: $(MAIN)

clean:
	@rm -f $(MAIN)

$(MAIN): $(SRCS)
	@$(CC) $(CFLAGS) -g -o $(MAIN) $(SRCS) -pthread
