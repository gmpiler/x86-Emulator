#ifndef INSTR_H
#define INSTR_H

#include "gemu.h"

typedef void instruction_func_t(Emulator*);

extern instruction_func_t *instructions[256];

void
init_instructions(void);

void
short_jump(Emulator *emu);

void
near_jump(Emulator* emu);

#endif