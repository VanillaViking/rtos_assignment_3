CC = gcc
CFLAGS = -Wall -lpthread -lrt -g

all: 
	$(CC) $(CFLAGS) -o assign3  Assignment3_template_Prg_1.c  
