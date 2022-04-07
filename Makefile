CC = gcc
CFLAGS = -g -I -std=gnu99 -lpthread -lm -lrt
all: oss uprocess

oss: config.h oss.c
	$(CC) -o $@ $^ $(CFLAGS)

uprocess: config.h uprocess.c
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm oss uprocess osslogfile