#include "EnumerationGraph.h"
#include <boost/graph/connected_components.hpp>
#include <iostream>

/**
 * This constructor decodes the graph6 format: http://users.cecs.anu.edu.au/~bdm/data/formats.txt
 * It assumes the graph is smaller than 64 nodes and the files contains no header!
 */
EnumerationGraph::EnumerationGraph(std::string graph_string, double t_test) {
    graph_name = graph_string;
    graph_size = int(graph_string[0]) - 63; // size < 64!
    graph_string.erase(graph_string.begin());
    toughness_test = t_test;

    vertices.reserve(graph_size);
    for (size_t i = 0; i < graph_size; i++) {
        vertices.push_back(add_vertex(g));
    }

    int outer_counter = 1;
    int inner_counter = 0;
    for (char &c : graph_string) {
        std::bitset<6> bs(int(c) - 63);
        for (std::size_t i = bs.size(); i-- > 0;) {
            if (bs[i]) {
                add_edge(vertices[outer_counter], vertices[inner_counter], g);
            }
            if (inner_counter == outer_counter - 1) {
                outer_counter++;
                inner_counter = 0;
            } else {
                inner_counter++;
            }
        }
    }
}

/**
 * This function checks the desired criteria of the graph, and prints a counterexample (graph name + pair
 * of vertices) if it is found.
  * @param subset_pairs contains the sets used to calculate the toughness
  * @param prev_ham_map is a hashmap containing useful hamilton path of the previous graph
  * @param prev_cut_set_map is a hashmap containing useful cuts of the previous graph
  * @return
  */
std::pair<ham_map_t, cut_set_map_t> EnumerationGraph::solve(const subset_pairs_t &subset_pairs,
                                                            ham_map_t &prev_ham_map, cut_set_map_t &prev_cut_set_map) {

    //Define local variables
    std::vector<bool> in_subgraph;
    in_subgraph.resize(graph_size, true);
    Filtered f(g, keep_all{}, [&](Vertex v) { return in_subgraph[v]; });
    bool low_tough = false;
    ham_map_t ham_map;
    cut_set_map_t cut_set_map;
    std::unordered_map<int, std::vector<std::vector<bool>>> current_sets;
    std::size_t pair1;
    std::size_t pair2;

    /**
     *  Lambda function that calculates the toughness based on the cuts provided in arr,
    *   set_size contains the size of the cuts in arr
     *   write_cut_sets indicates whether a found solution is written to current_subsets
     */
    auto test_toughness = [&] (const std::vector<std::vector<bool>> &arr, const int set_size, bool write_cut_sets) {
        std::vector<int> component(graph_size);
        for (auto &row : arr) {  //loop over |S|
            // update in_subgraph, which changes the filtered graph f
            in_subgraph = row;
            int comp_count = connected_components(f, &component[0]);

            // Subtract 1 for each component containing pair1 or pair2.
            if (in_subgraph[pair1]) {
                if (in_subgraph[pair2]) {
                    if (component[pair1] == component[pair2]) {
                        comp_count--;
                    } else {
                        comp_count -= 2;
                    }
                } else {
                    comp_count--;
                }
            } else if (in_subgraph[pair2]) {
                comp_count--;
            }

            if (comp_count > 0) {
                // (2*|S| + 1) / (2 omega) < 2
                if ((2 * (graph_size - (double) set_size) + 1) / (2.0 * comp_count) < toughness_test) {
                    cut_set_map.emplace(std::make_pair(pair1, pair2), std::make_pair(set_size, in_subgraph));
                    if (write_cut_sets) {
                        current_sets[set_size].push_back(in_subgraph);
                    }
                    return true;
                }
            }
        }
        return false;
    };

    for (pair1 = 0; pair1 < graph_size - 1; pair1++) {
        for (pair2 = pair1 + 1; pair2 < graph_size; pair2++) {
            std::pair<size_t, size_t> pair = std::make_pair(pair1, pair2);

            low_tough = false;
            for (const auto &kv: current_sets) {
                low_tough = test_toughness(kv.second, kv.first, false);
                if (low_tough) {
                    break;
                }
            }
            if (low_tough) {
                continue;
            }

            // Check ham_path of previous solution
            auto iterator = prev_ham_map.find(pair);
            if (iterator != prev_ham_map.end()) {
                Path path = iterator -> second;
                if (check_hamilton_path(path)) {
                    ham_map.emplace(pair, path);
                    continue;
                }
            } else {
                auto value = prev_cut_set_map[pair];
                int set_size;
                std::vector<bool> subset;
                std::tie(set_size, subset) = value;
                auto subsets = {subset,};
                low_tough = test_toughness(subsets, set_size, false);
                if (low_tough) {
                    continue;
                }
            }

            Path path;
            if (exists_hamilton_path_helper(vertices[pair1], vertices[pair2], path)) {
                ham_map.emplace(pair, path);
                continue;
            }
//            Possible optimisation for graph of size 5,8,11,14,.... This should be removed from the other
//                subsets_4 = get_less_sets(graph_size, 4, pair1, pair2);
//                low_tough = test_toughness_2(in_subgraph, f, subsets_4, 4, pair1, pair2, cut_set_map, current_sets);
//                if (low_tough) {
//                    continue;
//                }

            for (const auto &kv: subset_pairs) {
                low_tough = test_toughness(kv.second, kv.first, true);
                if (low_tough) {
                    break;
                }
            }
            if (not low_tough) {
                std::cout << graph_name << " " << pair1 << " " << pair2 << std::endl;
            }
        }
    }
    return std::make_pair(ham_map, cut_set_map);
}
