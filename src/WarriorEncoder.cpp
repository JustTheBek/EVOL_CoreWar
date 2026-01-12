#include "WarriorEncoder.h"
#include "Config.h"
#include <fstream>

void initGenome(GAGenome& g) {
    auto& genome = static_cast<GA1DArrayGenome<int>&>(g);
    for (int i = 0; i < GENOME_SIZE; ++i) {
        genome[i] = GARandomInt(-100, 100);
    }
}

void writeWarrior(const GA1DArrayGenome<int>& g,
                  const std::string& filename) {

    std::ofstream out(filename); //creates file if it doesn't exist
    out << "; Evolved warrior\n";
    out << "ORG 0\n";

    for (int i = 0; i < GENOME_SIZE; i += INSTR_FIELDS) {
        out << "MOV "
            << g[i+2] << ", "
            << g[i+4] << "\n";
    }
}

