TARGET=test7

CC = gcc
LD = ld
AS = nasm
CFLAGS += -nostdlib -fno-asynchronous-unwind-tables -g -fno-stack-protector
LDFLAGS += --entry=start --oformat=binary -m elf_i386

all:
# asmファイルのコンパイル用
	${AS} -f bin ${TARGET}.asm

c:
# cファイルのコンパイル用
	gcc -c ${TARGET}.c $(CFLAGS) -m32 -fno-pie -o ${TARGET}.o
	${AS} -f elf crt0.asm -o crt0.o
	${LD} ${LDFLAGS} -Ttext 0x7c00 crt0.o ${TARGET}.o -o ${TARGET}

disasm:
	ndisasm -b 32 ${TARGET}

clean:
	-find | grep -v Makefile | grep -v README.md | grep -v crt0.asm |\
	grep -v test0.asm | \
	grep -v test1.asm | \
	grep -v test2.asm | \
	grep -v test3.asm | \
	grep -v test4.c | \
	grep -v test5.c | \
	grep -v test6.c | \
	grep -v test7.c | \
	xargs rm -frv
