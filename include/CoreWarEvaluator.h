#pragma once
#include <string>
#include <ga/ga.h>  // <- Include GALib core headers

struct MatchResult {
    int myScore;
    int oppScore;
    int wins;
    int ties;
    int losses;
};

MatchResult runMatch(const std::string& opponent);
float evaluateFitness(const GA1DArrayGenome<int>& genome);

