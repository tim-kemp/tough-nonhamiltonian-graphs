#include "EvolutionaryAlgorithm.h"
#include <random>
#include <iostream>
#include <boost/graph/copy.hpp>
#include "EnumerationAlgorithm.h"

/**
 * The constructor automatically performs the evolutionary algorithm by calling evolve.
 * @param graph_size desired order of the graph; must be larger than 0
 * @param iterations desired number of iterations; must be larger than 0
 */
EvolutionaryAlgorithm::EvolutionaryAlgorithm(int graph_size, int iterations) {
    if (graph_size > 0 and iterations > 0) {
        // seed the random number generator
        std::random_device dev;
        rng.seed(dev());
        // Generate graphs until nonzero fitness is obtained
        while (true) {
            graph = EvolutionGraph(graph_size, 0.5);
            if (graph.get_number_of_components() == 1 and not graph.exists_hamilton_path(0, graph_size - 1)) {
                break;
            }
        }
        for (size_t i = 2; i < graph_size; i++) {
            subsets.emplace_back(i, EnumerationAlgorithm::get_sets(graph_size, i));
        }
        std::tie(current_tough, cut_S) = graph.solve_mutation(subsets, 0, cut_S);
        std::cout << graph.get_name() << "," << current_tough << ",";
        evolve(iterations);
    } else {
        throw std::invalid_argument("graph size and iterations must be positive");
    }
}

/**
 * This function performs the evolutionary algorithm.
 * @param iterations: number of iterations of the run
 * @return the final toughness
 */
double EvolutionaryAlgorithm::evolve(int iterations) {
    double old_tough = current_tough;
    int final_counter = 0;
    for (int i=0; i < iterations; i++) {
        bool changed = nextGen();
        if (changed and current_tough > old_tough) {
            old_tough = current_tough;
            final_counter = i;
        }
        // The result that 2.25 is best for n <= 11 follows form our enumeration algorithm
        if (graph.graph_size <= 11 and current_tough >= 2.25) {
            break;
        }
    }
    std::string new_name = graph.get_name();
    std::cout << new_name << "," << current_tough << "," << final_counter << std::endl;
    return current_tough;
}

/**
 * Performs a single iteration, by mutating four times and selecting the fittest individual among offspring as well
 * as parent as next parent.
 * @return whether the current parent is also the next parent
 */
bool EvolutionaryAlgorithm::nextGen() {
    bool changed = false;
    mutation_t best_mutation{};
    double best_tough = 0;
    for (int i=0; i<4; i++) {
        mutation_t mutation = graph.mutate(rng);
        double new_tough;
        std::vector<bool> new_cut;

        std::tie(new_tough, new_cut) = graph.solve_mutation(
                subsets, current_tough, cut_S, mutation.addition);
        if (new_tough >= current_tough) {
            if (new_tough > best_tough) {
                if (new_tough > current_tough) {
                    cut_S = new_cut;
                }
                best_tough = new_tough;
                best_mutation = mutation;
                changed = true;
            }
        }
        graph.undo_mutation(mutation);
    }
    if (changed) {
        graph.perform_mutation(best_mutation);
        current_tough = best_tough;
        // Uncomment following line for trace of the algorithm
//        graph.print_mutation(best_mutation);

    }
    return changed;
}

