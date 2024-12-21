#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "gemu.h"
#include "gemu_functions.h"
#include "instructions.h"

#define MEM_SIZE (1024 * 1024)
char *registers_name[] = { "EAX", "EAX", "ECX", "EDX", "EBX", "ESP", "EBP", "ESI", "EDI" };

static void
read_binary(Emulator *emu, const char *filename)
{
    FILE *binary;

    binary = fopen(filename, "rb");
    if (binary == NULL) {
        printf("[!] cannot open the file %s.\n", filename);
        exit(1);
    }
    fread(emu->memory + 0x7c00, 1, 0x200, binary);
    fclose(binary);
}

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
    for(int i = 0; i < REGISTERS_COUNT; i++)
        printf("%s = %08x\n", registers_name[i], emu->registers[i]);
    printf("EIP = %08x\n", emu->eip);
}

void
assert_code(uint8_t code)
{
    printf("\n\nNot Implemented: %x\n", code);
}

int
main(int argc, char* argv[])
{
    Emulator *gemu;
    init_instructions();

    /* Create an emulator */
    gemu = create_emu(MEM_SIZE, 0x7c00, 0x7c00);

    /* fetch instructions */
    read_binary(gemu, argv[1]);

    while(gemu->eip < MEM_SIZE) {
        uint8_t code = get_code8(gemu, 0);
        printf("EIP = %x, Code = %02x\n", gemu->eip, code);
        if(instructions[code] == NULL) {
            assert_code(code);
            break;
        }

        instructions[code](gemu);

        if(gemu->eip == 0x00) break;
    }

    dump_registers(gemu);
    free_emu(gemu);

    return 0;
}
