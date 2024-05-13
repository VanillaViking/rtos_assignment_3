CC = gcc
CFLAGS = -Wall -lpthread -lrt

all:
	$(CC) $(CFLAGS) -o assign3_part1  Assignment3_template_Prg_1.c
	$(CC) $(CFLAGS) -o assign3_part2  Assignment3_template_Prg_2.c
