#pragma once
#include <ga/ga.h>
#include <string>


// Biased initialization and Dwarf-based mutation
void initGenome(GAGenome& g);
void initGenomeFromDwarf(GAGenome& g, int generation = 0);
int mutateGenomeFromDwarf(GA1DArrayGenome<int>& genome, int generation);

// Helper functions to write warriors
int getOpcode(int val);
char getAddrMode(int val);
void writeWarrior(const GA1DArrayGenome<int>& g, const std::string& filename);
