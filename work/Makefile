TEST=test3

CC = gcc
CFLAGS += -Wall

LD = ld

all:
	$(CC) $(CFLAGS) gemu.c gemu_functions.c instructions.c modrm.c -o gemu

run:
	./gemu ../src/${TEST}