#include "WarriorEncoder.h"
#include "Config.h"
#include <fstream>
#include <iostream>
#include <string>
#include <algorithm> // for std::max

// ======================================================================
//  CoreWar opcode list (ICWS'94 compliant)
//
//  Order matters: genome stores opcode as integer index into this table
// ======================================================================
const char* opcodeStr[] = {
    "MOV","ADD","SUB","MUL","DIV","MOD",
    "JMP","JMZ","JMN","DJN",
    "SPL",
    "SEQ","SNE","SLT",
    "NOP",
    "DAT"
};
const int numOpcodes = sizeof(opcodeStr) / sizeof(opcodeStr[0]);

// ======================================================================
//  EXECUTION SAFETY CONSTANTS
//
//  First SAFE_CODE_LEN instructions are allowed to execute.
//  Everything beyond that is considered "data / payload zone".
//
//  This prevents DAT from ever being executed by our own instruction
//  pointer while still allowing DAT bombing.
// ======================================================================
constexpr int SAFE_CODE_LEN = 6;

// ======================================================================
//  Dwarf template genome
//  Used only as a *starting bias*, not a hard template
//
//  Each instruction = 5 ints:
//   [ opcode | A_mode | A_val | B_mode | B_val ]
// ======================================================================
const int dwarfGenome[] = {
    1, 4, 3, 0, 0,    // ADD.AB #4, $3
    0, 3, 2, 0, 0,    // MOV.I $3, @2
    6, 0, -2, 0, 0,   // JMP.B $-2, $0
    15, 0, 0, 0, 0,  // DAT.F #0, #0
    15, 0, 0, 0, 0   // DAT.F #0, #0
};

// ======================================================================
//  Helper: clamp operands safely
//
//  DAT:
//    - allowed to reference full core (bomb payload)
//
//  Non-DAT:
//    - limited range to avoid chaotic self-destruction
// ======================================================================
inline int safeOperand(int val, bool isDAT) {
    if(isDAT)
        return (val % CORESIZE + CORESIZE) % CORESIZE;
    else
        return (val % 100 + 100) % 100;
}

// ======================================================================
//  Decode opcode index safely from genome
// ======================================================================
int getOpcode(int val) {
    int op = val % numOpcodes;
    if(op < 0) op += numOpcodes;
    return op;
}

// ======================================================================
//  Decode addressing mode
//
//  DAT is *forced* to immediate mode to prevent indirect suicide.
// ======================================================================
char getAddrMode(int val, bool safeDAT = false) {
    static const char modes[] = {'#','$','@','<','>','*'};

    if(safeDAT)
        return '#'; // DAT payload only, never indirect

    int idx = val % 6;
    if(idx < 0) idx += 6;
    return modes[idx];
}

// ======================================================================
//  Initialize genome using Dwarf bias
//
//  Early generations:
//   - very small mutations
//
//  Later generations:
//   - more freedom
// ======================================================================
void initGenomeFromDwarf(GAGenome& g, int generation) {
    auto& genome = static_cast<GA1DArrayGenome<int>&>(g);
    const int templateSize = sizeof(dwarfGenome) / sizeof(int);

    for(int i = 0; i < genome.length(); ++i) {
        bool isOpcode = (i % INSTR_FIELDS == 0);
        bool isDAT = false;

        if(i < templateSize) {
            genome[i] = dwarfGenome[i];
            if(isOpcode)
                isDAT = (genome[i] == 15);

            // Gentle operand mutations early
            if(!isOpcode) {
                int delta = (generation < 5)
                    ? GARandomInt(-1,1)
                    : GARandomInt(-3,3);
                genome[i] = safeOperand(genome[i] + delta, isDAT);
            }

            // Opcode drift only after generation 10
            if(isOpcode && generation >= 10) {
                genome[i] = std::max(0, genome[i] + GARandomInt(-1,1));
            }
        } else {
            // Fill remaining genome with safe defaults
            int r = GARandomInt(0,100);
            if(r < 45) genome[i] = 0;       // MOV
            else if(r < 70) genome[i] = 6;  // JMP
            else if(r < 85) genome[i] = 10; // SPL
            else genome[i] = 15;            // DAT

            isDAT = (genome[i] == 15);
            if(!isOpcode)
                genome[i] = safeOperand(genome[i], isDAT);
        }
    }
}

// ======================================================================
//  GA initializer
// ======================================================================
void initGenome(GAGenome& g) {
    initGenomeFromDwarf(g, 0);
}

// ======================================================================
//  Write warrior (SAFE + LETHAL + SCANNING)
//
//  This version allows:
//   - DAT bombing outside SAFE_CODE_LEN
//   - SPL flooding forward
//   - JMP loops inside SAFE_CODE_LEN
//   - MOV-based scanning of opponent
//
//  Safety constraints prevent suicide:
//   - No DAT in executable code
//   - Indirect addressing (@, <, >, *) only outside code
//   - Arithmetic only modifies safe memory
// ======================================================================
void writeWarrior(const GA1DArrayGenome<int>& g, const std::string& filename) {
    std::ofstream out(filename);
    if(!out) {
        std::cerr << "Error opening " << filename << "\n";
        return;
    }

    out << "; Evolved warrior\n";
    out << "; assert CORESIZE==" << CORESIZE << "\n";
    out << "ORG 0\n";

    const int instrCount = g.length() / INSTR_FIELDS;

    for(int i = 0; i + INSTR_FIELDS - 1 < g.length(); i += INSTR_FIELDS) {
        int instrIndex = i / INSTR_FIELDS;

        // ==============================================================
        // 1) Opcode decode
        // ==============================================================
        int opIdx = getOpcode(g[i]);
        std::string opcode = opcodeStr[opIdx];
        bool isDAT = (opcode == "DAT");

        // ==============================================================
        // 2) HARD RULE: Executable code must NEVER kill itself
        // ==============================================================
        if(isDAT && instrIndex < SAFE_CODE_LEN) {
            opcode = "NOP";
            isDAT = false;
        }

        // ==============================================================
        // 3) Addressing modes
        // ==============================================================
        char am = '#';
        char bm = '#';

        if(instrIndex < SAFE_CODE_LEN) {
            // Executable region: only safe modes
            static const char execModes[] = {'#', '$'};
            am = execModes[GARandomInt(0,1)];
            bm = execModes[GARandomInt(0,1)];
        } else {
            // Weapon region: allow indirect for attacks
            static const char bombModes[] = {'#', '$', '@'};
            am = bombModes[GARandomInt(0,2)];
            bm = bombModes[GARandomInt(0,2)];
        }

        // ==============================================================
        // 4) Operands
        // ==============================================================
        int av = 0;
        int bv = 0;

        if(isDAT) {
            // ----------------------------------------------------------
            // DAT = BOMB
            // - Must point outside executable code
            // - Random far target improves attack chance
            // ----------------------------------------------------------
            am = '#';
            av = GARandomInt(SAFE_CODE_LEN + 20, CORESIZE - 1);
        } else {
            if(instrIndex < SAFE_CODE_LEN) {
                // Short offsets for loops and scanning
                av = (g[i+2] % 20 + 20) % 20;
                bv = (g[i+4] % 20 + 20) % 20;

                // ------------------------------------------------------
                // MOV-based scanning: occasionally read opponent memory
                // ------------------------------------------------------
                if(opcode == "MOV" && GARandomFloat() < 0.3f) {
                    // Scan a bit further ahead
                    bv = SAFE_CODE_LEN + GARandomInt(20, 50);
                }
            } else {
                // Weapon region: large offsets
                av = (g[i+2] % 400 + 400) % 400;
                bv = (g[i+4] % 400 + 400) % 400;
            }
        }

        // ==============================================================
        // 5) Arithmetic safety
        // ==============================================================
        if(instrIndex < SAFE_CODE_LEN &&
           (opcode == "ADD" || opcode == "SUB" ||
            opcode == "MUL" || opcode == "DIV" ||
            opcode == "MOD")) {
            // Only modify memory outside executable region
            bm = '$';
            bv = SAFE_CODE_LEN + GARandomInt(0, 30);
        }

        // ==============================================================
        // 6) SPL flooding (controlled)
        // ==============================================================
        if(opcode == "SPL") {
            if(instrIndex >= SAFE_CODE_LEN - 1) {
                // Too late to fork safely → neutralize
                opcode = "NOP";
            } else {
                am = '$';
                // Fork forward inside executable region
                av = instrIndex + GARandomInt(1, SAFE_CODE_LEN - instrIndex - 1);
            }
        }

        // ==============================================================
        // 7) Optional harmless NOP insertion (small % chance)
        // ==============================================================
        if(opcode != "DAT" && GARandomFloat() < 0.05f) {
            out << "NOP.F #0, #0\n";
            continue;
        }

        // ==============================================================
        // 8) Emit instruction (ICWS'94 compliant)
        // ==============================================================
        if(opcode == "JMP" || opcode == "SPL") {
            out << opcode << " " << am << av << "\n";
        }
        else if(opcode == "NOP") {
            out << "NOP.F #0, #0\n";
        }
        else if(isDAT) {
            out << "DAT " << am << av << "\n";
        }
        else {
            out << opcode << " " << am << av << ", " << bm << bv << "\n";
        }
    }
}

