#include <iostream>
#include <string>
#include <boost/graph/graphviz.hpp>
#include "EvolutionaryAlgorithm.h"
#include "EnumerationAlgorithm.h"

/**
 * Runs the algorithm 10000 times and prints the output more readable
 * @param graph_size
 * @param iterations
 */
void run_evolutionary_alg(int graph_size, int iterations) {
    std::cout << std::fixed;
    std::cout << std::setprecision(5);
    for (int i = 0; i < 10000; i++) {
        EvolutionaryAlgorithm survivor(graph_size, iterations);
    }
}


int main(int argc, char** argv) {
    // Insert path to graph files as string below, or pass as argument.
    // Input data: http://users.cecs.anu.edu.au/~bdm/data/
    std::string file_path = "/home/tim/CLionProjects/thesis/graphs/graph9c.g6";
    std::string file_path_chordal = "/home/tim/CLionProjects/thesis/graphs/chordal6.g6";

    EnumerationAlgorithm::readFile(file_path);
    EnumerationAlgorithm::readChordalFile(file_path_chordal);

    run_evolutionary_alg(8, 10000);
    run_evolutionary_alg(11, 1000000);

    return 0;
}
