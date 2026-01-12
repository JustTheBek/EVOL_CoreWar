#include "CoreWarEvaluator.h"
#include "WarriorEncoder.h"
#include "Config.h"

#include <fstream>
#include <vector>
#include <cmath>
#include <cstdlib>

//relative path from build folder, TODO: think about better solution
const std::string pmars= "../pmars-0.9.4/src/build/pmars";

MatchResult runMatch(const std::string& opponent) {
    MatchResult r{0,0,0,0,0};

    std::string cmd =
        pmars + " -r " + std::to_string(ROUNDS) +
        " -b -o ../tmp/ga.red " + opponent +
        " > ../tmp/result.txt";

    // Run the match
    int ret = system(cmd.c_str());
    if (ret != 0) {
        std::cerr << "Warning: system call returned " << ret << "\n";
    }

    // Ensure the result file exists (create if missing)
    std::ofstream resultFile("../tmp/result.txt", std::ios::app);
    resultFile.close();

    std::ifstream file("../tmp/result.txt");
    std::string line;

    while (std::getline(file, line)) {

        if (line.find("ga.red") != std::string::npos &&
            line.find("scores") != std::string::npos) {
            sscanf(line.c_str(), "%*[^s]scores %d", &r.myScore);
        }

        if (line.find(opponent) != std::string::npos &&
            line.find("scores") != std::string::npos) {
            sscanf(line.c_str(), "%*[^s]scores %d", &r.oppScore);
        }

        if (line.find("Results:") != std::string::npos) {
            sscanf(line.c_str(),
                   "Results: %d %d %d",
                   &r.wins, &r.ties, &r.losses);
        }
    }

    return r;
}

float evaluateFitness(const GA1DArrayGenome<int>& genome) {
    writeWarrior(genome, "../tmp/ga.red");

    //relative path from build folder, TODO: think about better solution
    std::vector<std::string> opponents = {
        "../warriors/dwarf.red",
        "../warriors/Imp.red",
        "../warriors/paper.red"
    };

    std::vector<float> scores;
    float sum = 0.0f;

    for (const auto& opp : opponents) {
        MatchResult r = runMatch(opp);

        float f =
            (r.myScore - r.oppScore)
          + 5.0f * r.wins
          - 5.0f * r.losses;

        scores.push_back(f);
        sum += f;
    }

    float mean = sum / scores.size();

    float variance = 0.0f;
    for (float f : scores) {
        variance += (f - mean) * (f - mean);
    }
    variance /= scores.size();

    return mean - VARIANCE_LAMBDA * variance;
}

