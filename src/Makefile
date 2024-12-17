TARGET=test0

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
