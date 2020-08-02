#ifndef REFACTORED_THESIS_EVOLUTIONGRAPH_H
#define REFACTORED_THESIS_EVOLUTIONGRAPH_H


#include <random>
#include "Graph.h"
struct mutation_t {
    bool addition;
    Vertex vertex1;
    Vertex vertex2;
};

using subset_pairs_t = std::vector<std::pair<int, std::vector<std::vector<bool>>>>;

class EvolutionGraph : public Graph {

public:
    EvolutionGraph();
    EvolutionGraph(int size, double prob);
    std::pair<double,std::vector<bool>> solve_mutation(const subset_pairs_t &subset_pairs, double tough_required,
                                                       std::vector<bool> previous_cut, bool edge_addition= true);
    mutation_t mutate(std::mt19937 &rng);
    void perform_mutation(mutation_t &mutation);
    void undo_mutation(mutation_t &mutation);
    void print_mutation(mutation_t &mutation);
    std::string get_name();
    int get_number_of_components();
};


#endif //REFACTORED_THESIS_EVOLUTIONGRAPH_H
