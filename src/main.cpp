#include <ga/ga.h>
#include "CoreWarEvaluator.h"
#include "WarriorEncoder.h"
#include "Config.h"
#include <iostream>
#include <cstdlib> // for atof, atoi

// Default GA parameters
static int population = 20;
static int generations = 100;
static double mutation = 0.05;
static double crossover = 0.9;

static float fitnessWrapper(GAGenome& g);
static void parse_input_arguments(int argc, char* argv[]);

// GA fitness wrapper
float fitnessWrapper(GAGenome& g)
{
    auto& genome = static_cast<GA1DArrayGenome<int>&>(g);
    return evaluateFitness(genome);
}

void parse_input_arguments(int argc, char* argv[])
{
    // Parse command line arguments
    // Usage: ./corewar_ga <population> <generations> <mutation> <crossover>
    if (argc > 1) population = std::atoi(argv[1]);
    if (argc > 2) generations = std::atoi(argv[2]);
    if (argc > 3) mutation = std::atof(argv[3]);
    if (argc > 4) crossover = std::atof(argv[4]);

    std::cout << "GA parameters:\n";
    std::cout << "Population: " << population
              << ", Generations: " << generations
              << ", Mutation: " << mutation
              << ", Crossover: " << crossover << "\n";
}

int main(int argc, char* argv[])
{

    parse_input_arguments(argc, argv);

    // Create a genome and use Dwarf-based initialization
    GA1DArrayGenome<int> genome(GENOME_SIZE, fitnessWrapper);
    genome.initializer(initGenome);

    // Configure GA
    GASimpleGA ga(genome);
    ga.populationSize(population);
    ga.nGenerations(generations);
    ga.pMutation(mutation);
    ga.pCrossover(crossover);

    // Run GA
    ga.evolve();

    // Write best warrior
    auto& best = static_cast<GA1DArrayGenome<int>&>(ga.population().best());
    writeWarrior(best, "best.red");

    std::cout << "Best fitness: " << best.score() << std::endl;

    return 0;
}
