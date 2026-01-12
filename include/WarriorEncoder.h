#pragma once
#include <ga/ga.h>
#include <string>

void initGenome(GAGenome& g);
void writeWarrior(const GA1DArrayGenome<int>& g,
                  const std::string& filename);

