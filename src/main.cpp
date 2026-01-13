#include <ga/ga.h>
#include "CoreWarEvaluator.h"
#include "WarriorEncoder.h"
#include "Config.h"
#include <iostream>

// GA fitness wrapper
float fitnessWrapper(GAGenome& g) {
    auto& genome = static_cast<GA1DArrayGenome<int>&>(g);
    return evaluateFitness(genome);
}

int main() {
    // Create a genome and use Dwarf-based initialization
    GA1DArrayGenome<int> genome(GENOME_SIZE, fitnessWrapper);
    genome.initializer(initGenome); // now Dwarf-based

    GASimpleGA ga(genome);
    ga.populationSize(20);
    ga.nGenerations(20);
    ga.pMutation(0.05);   // mutation probability per gene
    ga.pCrossover(0.9);   // crossover probability

    // Run GA
    ga.evolve();

    // Write best warrior
    auto& best = static_cast<GA1DArrayGenome<int>&>(ga.population().best());
    writeWarrior(best, "best.red");

    std::cout << "Best fitness: " << best.score() << std::endl;
}
