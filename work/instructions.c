#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "instructions.h"
#include "gemu.h"
#include "gemu_functions.h"
#include "modrm.h"

instruction_func_t* instructions[256];

void
short_jump(Emulator *emu)
{
    int8_t diff = get_sign_code8(emu, 1);
    emu->eip += (diff + 2);
}

void
near_jump(Emulator* emu)
{
    int32_t diff = get_sign_code32(emu, 1);
    emu->eip += (diff + 5);
}

/* opecode 0xB8．32bit immをレジスタに書き込む */
void
mov_r32_imm32(Emulator *emu)
{
    uint8_t reg = get_code8(emu, 0) - 0xB8;
    uint32_t value = get_code32(emu, 1);
    emu->registers[reg] = value;
    emu->eip += (1 + 4);
}

/* opecode 0xC7．32bit immをModR/Mで指定されたレジスタ/メモリに書き込む */
static void
mov_rm32_imm32(Emulator *emu)
{
    emu->eip += 1;
    ModRM modrm;
    parse_modrm(emu, &modrm);
    uint32_t value = get_code32(emu, 0);
    emu->eip += 4;
    set_rm32(emu, &modrm, value);
}

static void
mov_r32_rm32(Emulator *emu)
{
    emu->eip += 1;
    ModRM modrm;
    parse_modrm(emu, &modrm);
    uint32_t rm32 = get_rm32(emu, &modrm);
    set_r32(emu, &modrm, rm32);
}

static void
mov_rm32_r32(Emulator *emu)
{
    emu->eip += 1;
    ModRM modrm;
    parse_modrm(emu, &modrm);
    uint32_t r32 = get_r32(emu, &modrm);
    set_rm32(emu, &modrm, r32);
}

static void
add_rm32_r32(Emulator *emu)
{
    emu->eip += 1;
    ModRM modrm;
    parse_modrm(emu, &modrm);
    uint32_t r32    = get_r32(emu, &modrm);
    uint32_t rm32   = get_rm32(emu, &modrm);
    set_rm32(emu, &modrm, rm32 + r32);
}

static void
add_rm32_imm8(Emulator *emu, ModRM *modrm)
{
    uint32_t rm32 = get_rm32(emu, modrm);
    uint32_t imm8 = (int32_t)get_sign_code8(emu, 0);
    emu->eip += 1;
    set_rm32(emu, modrm, rm32 + imm8);
}

/* 0x83 /5 ib(imm byte) */
static void
sub_rm32_imm8(Emulator *emu, ModRM* modrm)
{
    uint32_t rm32 = get_rm32(emu, modrm);
    uint32_t imm8 = (int32_t)get_sign_code8(emu, 0);
    emu->eip += 1;
    uint64_t result = (uint64_t)rm32 - (uint64_t)imm8;
    set_rm32(emu, modrm, result);
    update_eflags_sub(emu, rm32, imm8, result);
}

static void
cmp_rm32_imm8(Emulator* emu, ModRM *modrm)
{
    uint32_t rm32 = get_rm32(emu, modrm);
    uint32_t imm8 = (int32_t)get_sign_code8(emu, 0);
    emu->eip += 1;
    uint64_t result = (uint64_t)rm32 - (uint64_t)imm8;
    update_eflags_sub(emu, rm32, imm8, result);
}

static void
code_83(Emulator *emu)
{
    emu->eip += 1;
    ModRM modrm;
    parse_modrm(emu, &modrm);

    switch(modrm.opecode) {
        case 0:
            add_rm32_imm8(emu, &modrm);
            break;
        case 5:
            sub_rm32_imm8(emu, &modrm);
            break;
        case 7:
            cmp_rm32_imm8(emu, &modrm);
            break;
        default:
            printf("[!] Not Implemented: 83% /%d\n", modrm.opecode);
            exit(1);
    }
}

static void
inc_rm32(Emulator *emu, ModRM *modrm)
{
    uint32_t value = get_rm32(emu, modrm);
    set_rm32(emu, modrm, value + 1);
}

static void
code_ff(Emulator *emu)
{
    emu->eip += 1;
    ModRM modrm;
    parse_modrm(emu, &modrm);

    switch(modrm.opecode) {
        case 0:
            inc_rm32(emu, &modrm);
            break;
        default:
            printf("[!] Not Implemented: FF /%d\n", modrm.opecode);
            exit(1);
    }
}

static void
push_r32(Emulator *emu)
{
    uint8_t reg = get_code8(emu, 0) - 0x50;
    push32(emu, get_register32(emu, reg));
    emu->eip += 1;
}

static void
pop_r32(Emulator *emu)
{
    uint8_t reg = get_code8(emu, 0) - 0x58;
    set_register32(emu, reg, pop32(emu));
    emu->eip += 1;
}

static void
push_imm32(Emulator *emu)
{
    uint32_t value = get_code32(emu, 1);
    push32(emu, value);
    emu->eip += 5;
}

static void
push_imm8(Emulator *emu)
{
    uint8_t value = get_code8(emu, 1);
    push32(emu, value);
    emu->eip += 2;
}

static void
call_rel32(Emulator *emu)
{
    int32_t diff = get_sign_code32(emu, 1);
    push32(emu, emu->eip + 5);
    emu->eip += (diff + 5);
}

static void
ret(Emulator *emu)
{
    emu->eip = pop32(emu);
}

static void
leave(Emulator *emu)
{
    uint32_t ebp = get_register32(emu, EBP);
    set_register32(emu, ESP, ebp);
    set_register32(emu, EBP, pop32(emu));

    emu->eip += 1;
}

static void
cmp_r32_rm32(Emulator* emu)
{
    emu->eip += 1;
    ModRM modrm;
    parse_modrm(emu, &modrm);
    uint32_t r32 = get_r32(emu, &modrm);
    uint32_t rm32 = get_rm32(emu, &modrm);
    uint64_t result = (uint64_t)r32 - (uint64_t)rm32;
    update_eflags_sub(emu, r32, rm32, result);
}

#define DEFINE_JX(flag, is_flag) \
static void j ## flag(Emulator *emu) { \
    int diff = is_flag(emu) ? get_sign_code8(emu, 1) : 0; \
    emu->eip += (diff + 2); \
} \
static void jn ## flag(Emulator *emu) { \
    int diff = is_flag(emu) ? 0 : get_sign_code8(emu, 1); \
    emu->eip += (diff + 2); \
}

DEFINE_JX(c, is_carry)
DEFINE_JX(z, is_zero)
DEFINE_JX(s, is_sign)
DEFINE_JX(o, is_overflow)

#undef DEFINE_JX

static void
jl(Emulator* emu)
{
    int diff = (is_sign(emu) != is_overflow(emu)) ? get_sign_code8(emu, 1) : 0;
    emu->eip += (diff + 2);
}

static void
jle(Emulator* emu)
{
    int diff = (is_zero(emu) || (is_sign(emu) != is_overflow(emu))) ? get_sign_code8(emu, 1) : 0;
    emu->eip += (diff + 2);
}

static void
jng(Emulator *emu)
{
    int diff = (is_zero(emu) || (is_sign(emu) != is_overflow(emu))) ? get_sign_code32(emu, 1) : 0;
    emu->eip += (diff + 5);
}

/* prefixの次の1バイトがopcodeとなる */
static void
prefix_ext_opcode_to_2bytes(Emulator *emu)
{
    emu->eip += 1;
    uint8_t extended_code = get_code8(emu, 0);
    printf(">> 16bit Code: %02x\n", extended_code);
    instructions[extended_code](emu);
}

/* prefixの次の2バイトがopcodeとなる(Primary + Secondary opcode) */
static void
prefix_ext_opcode_to_3bytes(Emulator *emu)
{
    emu->eip += 1;
    uint8_t primary_code = get_code8(emu, 0);
    switch(primary_code) {
        case 0x8E:  // jng cw/cd
            jng(emu);
            break;
        default:
            printf("[!] Not implemented 0x0F + %02x\n", primary_code);
            exit(1);
    }
}

static void
nop(Emulator *emu)
{
    emu->eip += 1;
}

void
init_instructions(void)
{
    memset(instructions, 0, sizeof(instructions));
    instructions[0x01] = add_rm32_r32;
    instructions[0x0F] = prefix_ext_opcode_to_3bytes;

    instructions[0x3B] = cmp_r32_rm32;

    for(int i = 0; i < 8; i++)
        instructions[0x50 + i] = push_r32;
    for(int i = 0; i < 8; i++)
        instructions[0x58 + i] = pop_r32;

    instructions[0x66] = prefix_ext_opcode_to_2bytes;
    instructions[0x68] = push_imm32;
    instructions[0x6A] = push_imm8;

    instructions[0x70] = jo;
    instructions[0x71] = jno;
    instructions[0x72] = jc;
    instructions[0x73] = jnc;
    instructions[0x74] = jz;
    instructions[0x75] = jnz;
    instructions[0x78] = js;
    instructions[0x79] = jns;
    instructions[0x7C] = jl;
    instructions[0x7E] = jle;

    instructions[0x83] = code_83;
    instructions[0x89] = mov_rm32_r32;
    instructions[0x8B] = mov_r32_rm32;

    instructions[0x90] = nop;

    for(int i = 0; i < 8; i++)
        instructions[0xB8 + i] = mov_r32_imm32;

    instructions[0xC3] = ret;
    instructions[0xC7] = mov_rm32_imm32;
    instructions[0xC9] = leave;

    instructions[0xE8] = call_rel32;
    instructions[0xE9] = near_jump;
    instructions[0xEB] = short_jump;
    instructions[0xFF] = code_ff;
}