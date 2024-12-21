TEST=test2

CC = gcc
CFLAGS += -Wall

LD = ld

all:
	$(CC) $(CFLAGS) gemu.c gemu_functions.c instructions.c modrm.c -o gemu

run:
	./gemu ../src/${TEST}

clean:
	-find | grep -v Makefile | grep -v README.md | \
	grep -v gemu.c | grep -v gemu.h | \
	grep -v gemu_functions.c | grep -v gemu_functions.h | \
	grep -v instructions.c | grep -v instructions.h | \
	grep -v modrm.c | grep -v modrm.h | \
	xargs rm -frv
