#
# Austin Ng (ang24), Kelechi (kigwe)
#
# This is the Makefile for Lab 7, "Designing a Virtual Memory Manager".
#

CC = gcc
CFLAGS = -Wall
LD = gcc
LDFLAGS =

MAIN = vmm
SRCS = main.c vm_manager.c

all: $(MAIN)

clean:
	rm -f $(MAIN)

$(MAIN): $(SRCS)
	@$(CC) $(CFLAGS) -g -o $(MAIN) $(SRCS)
