CC = gcc
CFLAGS = -Wall -lpthread -lrt -g

all: queue.o 
	$(CC) $(CFLAGS) -o assign3_part1  Assignment3_template_Prg_1.c
	$(CC) $(CFLAGS) -o assign3_part2  Assignment3_template_Prg_2.c

queue.o: queue.c queue.h
	$(CC) $(CFLAGS) -c queue.c -o queue.o 
