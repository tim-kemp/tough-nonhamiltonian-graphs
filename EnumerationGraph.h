#ifndef REFACTORED_THESIS_ENUMERATIONGRAPH_H
#define REFACTORED_THESIS_ENUMERATIONGRAPH_H


#include "Graph.h"

struct hash_pair {
    template<class T1, class T2>
    size_t operator()(const std::pair<T1, T2> &p) const {
        auto hash1 = hash<T1>{}(p.first);
        auto hash2 = hash<T2>{}(p.second);
        return hash1 ^ hash2;
    }
};

using ham_map_t = std::unordered_map<std::pair<int, int>, Path, hash_pair>;
using cut_set_map_t = std::unordered_map<std::pair<int, int>, std::pair<int, std::vector<bool>>, hash_pair>;
using subset_pairs_t = std::vector<std::pair<int, std::vector<std::vector<bool>>>>;

class EnumerationGraph : public Graph {
    double toughness_test;

public:
    EnumerationGraph(std::string graph_string, double t_test);
    std::pair<ham_map_t, cut_set_map_t> solve(const subset_pairs_t &subset_pairs, ham_map_t &prev_ham_map,
                                              cut_set_map_t &write_cut_sets);
};


#endif //REFACTORED_THESIS_ENUMERATIONGRAPH_H
