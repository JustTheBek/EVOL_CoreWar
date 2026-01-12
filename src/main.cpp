#include <ga/ga.h>
#include "CoreWarEvaluator.h"
#include "WarriorEncoder.h"
#include "Config.h"
#include <iostream>

float fitnessWrapper(GAGenome& g) {
    auto& genome = static_cast<GA1DArrayGenome<int>&>(g);
    return evaluateFitness(genome);
}

int main() {
    // Create genome
    GA1DArrayGenome<int> genome(GENOME_SIZE, fitnessWrapper);
    genome.initializer(initGenome);

    // Create GA
    GASimpleGA ga(genome);
    ga.populationSize(10); //og:100
    ga.nGenerations(20); // og:200
    ga.pMutation(0.02);
    ga.pCrossover(0.9);

    // Run evolution
    ga.evolve();

    // Get the best individual from population
    auto& best =
        static_cast<GA1DArrayGenome<int>&>(ga.population().best());

    // Write best warrior
    writeWarrior(best, "best.red");

    std::cout << "Best fitness: " << best.score() << std::endl;
}
