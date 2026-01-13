#include "CoreWarEvaluator.h"
#include "WarriorEncoder.h"
#include "Config.h"

#include <fstream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <future>
#include <string>
#include <atomic>

const std::string pmars = "../pmars-0.9.4/src/build/pmars";
static std::atomic<std::int64_t> evalCounter{0};

// --------------------- Run a single match --------------------------------
MatchResult runMatchUnique(const std::string& warriorFile,
                           const std::string& opponent,
                           int idx)
{
    MatchResult r{0,0,0,0,0};
    std::string gaFile  = "../tmp/ga_" + std::to_string(idx) + ".red";
    std::string resultFile = "../tmp/result_" + std::to_string(idx) + ".txt";

    // Copy genome file
    {
        std::ifstream src(warriorFile, std::ios::binary);
        std::ofstream dst(gaFile, std::ios::binary);
        dst << src.rdbuf();
    }

    std::string cmd = pmars + " -r " + std::to_string(ROUNDS) +
                      " -s " + std::to_string(CORESIZE) +
                      " -b -o " + gaFile + " " + opponent +
                      " > " + resultFile + " 2>&1";

    int ret = system(cmd.c_str());
    if(ret!=0) { r.losses=1; return r; }

    std::ifstream file(resultFile);
    if(!file) { r.losses=1; return r; }

    std::string line;
    bool found=false;
    while(std::getline(file,line)) {
        if(line.find("Results:")!=std::string::npos) {
            int first, second, third;
            sscanf(line.c_str(), "Results: %d %d %d", &first,&second,&third);
            r.wins = first;
            r.losses = second;
            r.ties = third;
            found=true;
            break;
        }
    }
    if(!found) r.losses=1;
    return r;
}

// --------------------- Evaluate fitness ---------------------------------
float evaluateFitness(const GA1DArrayGenome<int>& genome) {
    std::int64_t id = evalCounter++;
    std::cout << "Eval " << id << "\n";

    std::string warriorFile="../tmp/ga_temp.red";
    writeWarrior(genome, warriorFile);

    std::vector<std::string> opponents = {
        "../warriors/dwarf.red",
        "../warriors/Imp.red",
        "../warriors/paper.red"
    };

    std::vector<std::future<MatchResult>> futures;
    for(size_t i=0;i<opponents.size();++i)
        futures.push_back(std::async(std::launch::async,
                                     runMatchUnique, warriorFile, opponents[i], (int)i));

    float sum=0;
    std::vector<float> scores;
    for(auto& fut : futures) {
        MatchResult r = fut.get();
        float f = static_cast<float>(r.wins)/ROUNDS
                + 0.5f*static_cast<float>(r.ties)/ROUNDS
                - static_cast<float>(r.losses)/ROUNDS;
        scores.push_back(f);
        sum += f;
    }

    float mean = sum / scores.size();
    float variance = 0.0f;
    for(float f : scores) variance += (f-mean)*(f-mean);
    variance /= scores.size();

    float rawFitness = mean - VARIANCE_LAMBDA*variance;

    std::cout << "Match scores: ";
    for(float f : scores) std::cout << f << " ";
    std::cout << "=> rawFitness=" << rawFitness << "\n";

    return rawFitness;
}
