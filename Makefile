CC=gcc
CFLAGS=-I. -Wall

main: main.o zycarver.o
	$(CC) -o main main.o zycarver.o -I.

clean:
	rm *.o
	rm main
