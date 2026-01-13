#include "WarriorEncoder.h"
#include "Config.h"
#include <fstream>
#include <iostream>
#include <string>

// List of CoreWar opcodes
const char* opcodeStr[] = {
    "MOV","ADD","SUB","MUL","DIV","MOD","JMP","JMZ","JMN","DJN","SPL",
    "SEQ","SNE","SLT","NOP","DAT"
};
const int numOpcodes = sizeof(opcodeStr)/sizeof(opcodeStr[0]);

// --------------------- Dwarf template genome ---------------------------
// Each 5-int instruction: opcode + 4 operands
const int dwarfGenome[] = {
    1, 4, 3, 0, 0,    // ADD.AB #4, $3
    0, 3, 2, 0, 0,    // MOV.I $3, @2
    6, 0, -2, 0, 0,   // JMP.B $-2, $0
    15, 0, 0, 0, 0,   // DAT.F #0, #0
    15, 0, 0, 0, 0    // DAT.F #0, #0
};

// --------------------- Safe Dwarf initialization -----------------------
void initGenomeFromDwarf(GAGenome& g, int generation) {
    auto& genome = static_cast<GA1DArrayGenome<int>&>(g);
    const int templateSize = sizeof(dwarfGenome)/sizeof(int);

    for(int i=0;i<genome.length();++i) {
        if(i < templateSize) {
            genome[i] = dwarfGenome[i];

            // Small tweaks on operands only for first generations
            if(i % INSTR_FIELDS != 0) { // operand
                int delta = (generation < 5) ? GARandomInt(-1,1) : GARandomInt(-3,3);
                genome[i] += delta;
            }

            // Optionally tweak opcode in later generations
            if(i % INSTR_FIELDS == 0 && generation >= 10) {
                int opcodeDelta = GARandomInt(-1,1);
                genome[i] = std::max(0, genome[i] + opcodeDelta);
            }
        } else {
            // Fill rest of genome with "safe" instructions
            int r = GARandomInt(0,100);
            if(r < 50) genome[i] = 0;     // MOV
            else if(r < 80) genome[i] = 6; // JMP
            else genome[i] = 15;           // DAT
        }
    }
}

// --------------------- Generic biased init (kept for GA API) -----------
void initGenome(GAGenome& g) {
    initGenomeFromDwarf(g, 0); // generation 0 = first population
}

// --------------------- Mutate with gradual exploration ----------------
int mutateGenomeFromDwarf(GA1DArrayGenome<int>& genome, int generation) {
    int mutations = 0;
    for(int i=0;i<genome.length();++i) {
        if(GARandomFloat() < 0.05) { // 5% chance per gene
            if(i % INSTR_FIELDS != 0) { // operand
                int delta = (generation < 10) ? GARandomInt(-1,1) : GARandomInt(-3,3);
                genome[i] += delta;
            } else { // opcode
                if(generation >= 10) {
                    int opcodeDelta = GARandomInt(-1,1);
                    genome[i] = std::max(0, genome[i] + opcodeDelta);
                }
            }
            mutations++;
        }
    }
    return mutations;
}

// --------------------- Helper functions --------------------------------
int getOpcode(int val) {
    int op = val % numOpcodes;
    if(op < 0) op += numOpcodes;
    return op;
}

char getAddrMode(int val) {
    static const char modes[] = {'#','$','@','<','>','*'};
    int idx = val % 6;
    if(idx<0) idx+=6;
    return modes[idx];
}

void writeWarrior(const GA1DArrayGenome<int>& g, const std::string& filename) {
    std::ofstream out(filename);
    if(!out) { std::cerr << "Error opening " << filename << "\n"; return; }

    out << "; Evolved warrior\n; assert CORESIZE==" << CORESIZE << "\n";
    out << "ORG 0\n";

    for(int i=0;i+INSTR_FIELDS-1<g.length();i+=INSTR_FIELDS) {
        int opIdx = getOpcode(g[i]);
        std::string opcode = opcodeStr[opIdx];

        char am = getAddrMode(g[i+1]);
        int av = g[i+2];
        char bm = getAddrMode(g[i+3]);
        int bv = g[i+4];

        if(opcode!="DAT") { av%=100; if(av<0) av+=100; bv%=100; if(bv<0) bv+=100; }
        else { av%=CORESIZE; if(av<0) av+=CORESIZE; bv%=CORESIZE; if(bv<0) bv+=CORESIZE; }

        if(opcode=="JMP" || opcode=="SPL") out << opcode << " " << am << av << "\n";
        else if(opcode=="DAT") out << opcode << " " << am << av << "\n";
        else out << opcode << " " << am << av << ", " << bm << bv << "\n";
    }
}
