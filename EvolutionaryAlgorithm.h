#ifndef THESIS_SINGLESURVIVOR_H
#define THESIS_SINGLESURVIVOR_H

#include "EvolutionGraph.h"
#include <random>

class EvolutionaryAlgorithm {
    std::mt19937 rng;
    std::vector<bool> cut_S;
public:
    EvolutionGraph graph;
    std::vector<std::pair<int, std::vector<std::vector<bool>>>> subsets;
    double current_tough{};
    explicit EvolutionaryAlgorithm(int graph_size, int iterations);
    double evolve(int iterations);
    bool nextGen();
};


#endif //THESIS_SINGLESURVIVOR_H
