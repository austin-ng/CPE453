#
# Yvan Gonzalez (yggonzal), Austin Ng (ang24), and Kelechi Igwe (kigwe)
#
# This is the Makefile for Lab 5, "Support for Lightweight Processes".
#

CC = gcc
CFLAGS = -Wall
LD =
LDFLAGS =

MAIN = liblwp.so
OBJS = lwp.o magic64.o
SRCS = snakemain.c numbersmain.c

all: 	$(MAIN)

clean:	
	rm -f $(MAIN) $(OBJS)

$(MAIN): lwp.o magic64.o
	@ar -rc $(MAIN) $(OBJS)

lwp.o: lwp.c
	@$(CC) $(CFLAGS) -c lwp.c -o lwp.o

magic64.o: magic64.S 
	@$(CC) $(CFLAGS) -c magic64.S -o magic64.o
