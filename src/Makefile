TARGET=test2

CC = gcc
LD = ld
AS = nasm
CFLAGS += -nostdlib -fno-asynchronous-unwind-tables -g -fno-stack-protector
LDFLAGS += --entry=func -m elf_i386 --oformat=binary

all:
# gcc -c -o ${TARGET}.o ${TARGET}.asm $(CFLAGS) -m32 -fno-pie
# ld $(LDFLAGS) ${TARGET}.o -m elf_i386 --oformat=binary
	${AS} -f bin ${TARGET}.asm

disasm:
	ndisasm -b 32 ${TARGET}

clean:
	-find | grep -v Makefile | grep -v README.md | \
	grep -v test0.asm | \
	grep -v test1.asm | \
	grep -v test2.asm | \
	xargs rm -frv
