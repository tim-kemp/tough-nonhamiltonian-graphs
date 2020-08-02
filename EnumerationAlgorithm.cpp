#include "EnumerationAlgorithm.h"
#include <iostream>
#include <boost/graph/graphviz.hpp>
#include <boost/function.hpp>
#include "EnumerationGraph.h"

/**
 * This function creates all possible subsets of a set {0, 1, 2, ..., length-1}. The subsets are returned in binary
 * representation, i.e. bitmask<i> denotes whether i is in the subset.
 */
std::vector<std::vector<bool>> EnumerationAlgorithm::get_sets(unsigned int length, unsigned int subset_size) {
    std::vector<std::vector<bool>> subsets;
    std::vector<bool> bitmask(subset_size, true);
    bitmask.resize(length, false);
    do {
        subsets.push_back(bitmask);
    } while (std::prev_permutation(bitmask.begin(), bitmask.end()));
    return subsets;
}

//
/**
 * Return sets requires to analyse to test tough < 2,
 * See table 4.2 of my thesis (note: below func defines those to keep)
 */
std::vector<int> EnumerationAlgorithm::get_set_size(int order) {
    switch(order) {
        case 5:
            return {2,4};
        case 6:
            return {3, 5};
        case 7:
            return {4, 6};
        case 8:
            return {3, 5, 7};
        case 9:
            return {4, 6, 8};
        case 10:
            return {5, 7, 9};
        case 11:
            return {4, 6, 8, 10};
        case 12:
            return {5, 7, 9, 11};
        default:
            throw std::invalid_argument("Function not implemented for this value");
    }
}

/**
 * The same as get_sets above, but returns only those subsets containing neither u or v.
 * Not used by default, can be used on order 8 11 14 etc..
 */
std::vector<std::vector<bool>> EnumerationAlgorithm::get_less_sets(
        unsigned int length, unsigned int subset_size, unsigned int u, unsigned int v) {
    std::vector<std::vector<bool>> subsets;
    std::vector<bool> bitmask(subset_size, true);
    bitmask.resize(length, false);
    do {
        if (not bitmask[u] and not bitmask[v]) {
            subsets.push_back(bitmask);
        }
    } while (std::prev_permutation(bitmask.begin(), bitmask.end()));
    return subsets;
}



/**
 * Performs the enumeration algorithm.
 * This method assumes all the graphs have the same order (to avoid redundant calls to get_sets)
 * It is also assumes the g6 format and that no header is present!
 */
void EnumerationAlgorithm::readFile(const std::string &filename, double toughness_test) {
    std::ifstream infile(filename);
    if (!infile) {
        std::cerr << "The file doesn't exist" << std::endl;
    } else {
        int graph_size = infile.peek() - 63;
        std::vector<std::pair<int, std::vector<std::vector<bool>>>> subsets;
        for (auto set_size : get_set_size(graph_size)) {
            subsets.emplace_back(set_size, get_sets(graph_size, set_size));
        }
        std::string line;
        ham_map_t ham_map;
        cut_set_map_t cut_set_map;
        while (std::getline(infile, line)) {
            EnumerationGraph my_graph(line, toughness_test);
            if (not my_graph.hasCompleteClosure(graph_size + 1)) {
                std::tie(ham_map, cut_set_map) = my_graph.solve(subsets, ham_map, cut_set_map);
            }
        }
    }
}

/**
 * Performs the enumeration algorithm for chordal graphs.
 * Very similar to the above function, included both without reuse to allow easy modifications only applicable to
 * chordal graphs.
 * This method assumes all the graphs have the same order (to avoid redundant calls to get_sets)
 * It is also assumes the g6 format and that no header is present!
 * Use toughness_test=1.751 to exclude graphs of order 1.75
 */
void EnumerationAlgorithm::readChordalFile(const std::string &filename, double toughness_test) {
    std::ifstream infile(filename);
    if (!infile) {
        std::cerr << "The file doesn't exist" << std::endl;
    } else {
        int graph_size = infile.peek() - 63;
        std::vector<std::pair<int, std::vector<std::vector<bool>>>> subsets;
        // note that these are too many for chordal graphs
        // can be optimised based on section 4.5.2 of my report depending on toughness_test and graph order
        for (size_t i = 2; i < graph_size; i++) {
            subsets.emplace_back(i, get_sets(graph_size, i));
        }
        std::string line;
        ham_map_t ham_map;
        cut_set_map_t cut_set_map;
        while (std::getline(infile, line)) {
            EnumerationGraph my_graph(line, toughness_test);
            if (not my_graph.hasCompleteClosure(graph_size + 1)) {
                std::tie(ham_map, cut_set_map) = my_graph.solve(subsets, ham_map, cut_set_map);
            }
        }
    }
}