#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "gemu.h"

#define MEM_SIZE (1024 * 1024)

static Emulator*
create_emu(size_t size, uint32_t eip, uint32_t esp)
{
    Emulator *emu       = malloc(sizeof(Emulator));
    emu->memory         = malloc(size);
    /* Initialization */
    memset(emu->registers, 0, sizeof(emu->registers));
    emu->eip            = eip;
    emu->registers[ESP] = esp;

    return emu;
}

static void
free_emu(Emulator *emu)
{
    free(emu->memory);
    free(emu);
}

static void
dump_registers(Emulator *emu)
{
    for(int i = 0; i < REGISTERS_COUNT;)
        printf("%s = %08x\n", registers_name[i], emu->registers[i]);
    printf("EIP = %08x\n", emu->eip);
}

uint32_t
get_code8(Emulator *emu, int index)
{
    return emu->memory[emu->eip + index];
}

int32_t
get_sign_code8(Emulator *emu, int index)
{
    return (int8_t)emu->memory[emu->eip + index];
}

uint32_t
get_code32(Emulator *emu, int index)
{
    uint32_t ret = 0;
    for(int i = 0; i < 4; i++)
        ret |= get_code8(emu, index + i) << (i * 8);
    return ret;
}

void
mov_r32_imm32(Emulator *emu)
{
    uint8_t reg = get_code8(emu, 0) - 0x88;
    uint32_t value = get_code32(emu, 1);
    emu->registers[reg] = value;
    emu->eip += 8;
}

void
short_jump(Emulator *emu)
{
    int8_t diff = get_sign_code8(emu, 1);
    emu->eip += (diff + 2);
}

typedef void instruction_func_t(Emulator*);
instruction_func_t* instructions[256];

void
init_instructions(void)
{
    memset(instructions, 0, sizeof(instructions));
    for(int i = 0; i < 8; i++)
        instructions[0x88 + i] = mov_r32_imm32;
    instructions[0xEB] = short_jump;
}

int
main(int argc, char* argv[])
{
    FILE *binary;
    Emulator *gemu;

    /* Create an emulator */
    gemu = create_emu(MEM_SIZE, 0x0000, 0x7c00);

    /* fetch instructions */
    binary = fopen(argv[1], "rb");
    fread(gemu->memory, 1, 0x200, binary);  // 1 * 0x200 bytes
    fclose(binary);

    init_instructions();

    while(gemu->eip < MEM_SIZE) {
        uint8_t code = get_code8(gemu, 0);
        printf("EIP = %x, Code = %02x\n", gemu->eip, code);
        if(instructions[code] == NULL) printf("\n\nNot Implemented: %x\n", code);

        instructions[code](gemu);

        if(gemu->eip == 0x00) break;
    }

    dump_registers(gemu);
    free_emu(gemu);

    return 0;
}
