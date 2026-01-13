#pragma once

// Each Core War instruction is encoded as 5 integers in the genome:
// [ opcode, A_mode, A_value, B_mode, B_value ]
constexpr int INSTR_FIELDS = 5;

// Total number of instructions in one warrior program
constexpr int INSTR_COUNT  = 15;

// Total genome length = instructions × fields per instruction
// This MUST match all encoder / mutation logic exactly
constexpr int GENOME_SIZE  = INSTR_FIELDS * INSTR_COUNT;


constexpr int ROUNDS = 50;
constexpr float VARIANCE_LAMBDA = 0.1f;

// CoreWar core size
constexpr int CORESIZE = 8000;