#include "Graph.h"
#include <random>
#include <boost/graph/connected_components.hpp>
#include <bitset>
#include <string>
#include <iostream>
#include <boost/graph/copy.hpp>
#include "EvolutionGraph.h"

EvolutionGraph::EvolutionGraph() = default;


/**
 * Constructor for the initial population of the EA.
 * Creates a graph of order n, where each possible edge is chosen to be included in the graph with probability prob.
 */
EvolutionGraph::EvolutionGraph(int size, double prob) {
    graph_size = size;
    vertices.reserve(graph_size);
    for (size_t i = 0; i < graph_size; i++) {
        vertices.push_back(add_vertex(g));
    }
    std::random_device dev;  // seed the random number generator
    std::mt19937 rng(dev());

    std::bernoulli_distribution coin_dist{prob};

    std::string g6string(1, (char) graph_size + 63);
    std::bitset<6> my_bit;
    size_t current_bit = 6;
    for (int j=1; j<graph_size; j++) {
        for (int i=0; i<j; i++) {
            if (coin_dist(rng)) {
                add_edge(vertices[i], vertices[j], g);
                my_bit.set(--current_bit, true);
            } else {
                my_bit.set(--current_bit, false);
            }
            if (current_bit == 0) {
                current_bit = 6;
                int int_value = (int) my_bit.to_ulong();
                g6string.push_back((char) (63 + int_value));
            }
        }
    }
    if (current_bit != 6) {
        while (current_bit > 0) {
            my_bit.set(--current_bit, false);
        }
        g6string.push_back((char) (63 + my_bit.to_ulong()));
    }
    g6string.push_back((char) (63 + my_bit.to_ulong()));
    graph_name = g6string;

}

 /**
  * This function is used to calculate the toughness in the evolutionary algorithm.
  * @param subset_pairs contains the sets used to calculate the toughness
  * @param tough_required is the minimum toughness to test that determines when to break the algorithm
  * @param previous_cut is a useful cut of the parent graph
  * @param edge_addition is a boolean indicating whether an edge is added or removed
  * @return
  */
std::pair<double,std::vector<bool>> EvolutionGraph::solve_mutation(
        const subset_pairs_t &subset_pairs, double tough_required, std::vector<bool> previous_cut, bool edge_addition) {
    std::vector<bool> in_subgraph;
    in_subgraph.resize(graph_size, true);
    Filtered f(g, keep_all{}, [&](Vertex v) { return in_subgraph[v]; });

//    std::unordered_map<int, std::vector<std::vector<bool>>> current_sets;
    size_t pair1 = 0;
    size_t pair2 = graph_size-1;
    double tough = 100; //arbitrary large number
    std::vector<bool> best_cut;

     /**
      *  Lambda function that calculates the toughness based on the cuts provided in arr,
      *   set_size contains the size of the cuts in arr
      */
     auto get_toughness = [&] (const std::vector<std::vector<bool>> &arr, const int set_size) {
         std::vector<int> component(graph_size);
         double tough = 100;
//    double new_tough;
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
                 // (2*|S| + 1) / (2 omega')
                 double new_tough = (2 * (graph_size - (double) set_size) + 1) / (2.0 * comp_count);
                 if (new_tough < tough) {
                     tough = new_tough;
                     best_cut = row;
                 }
                 if (tough < tough_required) {
                     return tough;
                 }
             }
         }
         return tough;
     };

    //Check the previous solution
    std::vector<std::vector<bool>> solution_wrapper;
    solution_wrapper.push_back(previous_cut);
    int previous_cut_size =  std::count(previous_cut.begin(), previous_cut.end(), true);
    tough = std::min(tough, get_toughness(solution_wrapper, previous_cut_size));
    if (tough < tough_required) {
        return make_pair(tough, best_cut);
    }

    //Only check hamilton path if a new edge is added, not when the mutation deletes one.
    if (edge_addition and exists_hamilton_path(pair1, pair2)) {
        return make_pair(0, in_subgraph);
    }

    std::vector<bool> recent_cut;
    double new_tough;


    for (const auto &kv: subset_pairs) {
        new_tough = get_toughness(kv.second, kv.first);
        if (new_tough < tough) {
            tough = new_tough;
            best_cut = recent_cut;
        }
        if (tough < tough_required) {
            return make_pair(tough, best_cut);
        }
    }

    return make_pair(tough, best_cut);
}


/**
 * This function perform a random mutation. Fifty per cent chance to add an edge such that each possible edge is equally
 * probable. Otherwise remove an edge such that each edge is equally probable to be removed
 * @param rng A random number generator
 * @return The mutation that is executed
 */
mutation_t EvolutionGraph::mutate(std::mt19937 &rng) {
    mutation_t mutation;
    std::bernoulli_distribution coin_dist{0.50};
    if (coin_dist(rng)) { //add edge
        std::uniform_int_distribution<std::mt19937::result_type> pick_vertex(0, graph_size-1);
        while (true) {
            Vertex x = pick_vertex(rng);
            Vertex y = pick_vertex(rng);
            if (x != y and not edge(x, y, g).second) {
                add_edge(x, y, g);
                // True means an edge is added
                mutation = {true, x, y};
                break;
            }
        }
    } else { //delete edge
        std::uniform_int_distribution<std::mt19937::result_type> pick_vertex(0, graph_size-1);
        while (true) {
            Vertex x = pick_vertex(rng);
            Vertex y = pick_vertex(rng);
            if (x != y and edge(x, y, g).second) {
                remove_edge(x, y, g);
                // False means an edge is deleted
                mutation = {false, x, y};
                break;
            }
        }
    }
    return mutation;
}

/**
 * Executed the passed mutation
 * @param mutation contains two vertices and a boolean indicating whether an edge is added or removed
 */
void EvolutionGraph::perform_mutation(mutation_t &mutation) {
    if (mutation.addition) {
        add_edge(mutation.vertex1, mutation.vertex2, g);
    } else {  //restore edge
        remove_edge(mutation.vertex1, mutation.vertex2, g);
    }
}

/**
 * Undo the passed mutation, reverse of the above function
 * @param mutation contains two vertices and a boolean indicating whether an edge is added or removed
 */
void EvolutionGraph::undo_mutation(mutation_t &mutation) {
    if (mutation.addition) { //undo edge addition
        remove_edge(mutation.vertex1, mutation.vertex2, g);
    } else {  //restore edge
        add_edge(mutation.vertex1, mutation.vertex2, g);
    }
}

/**
 * Prints the mutation. Can be used to obtain a trace of the algorithm.
 * @param mutation contains two vertices and a boolean indicating whether an edge is added or removed
 */
void EvolutionGraph::print_mutation(mutation_t &mutation) {
    if (mutation.addition) { //undo edge addition
        std::cout << mutation.vertex1 << " " << mutation.vertex2 << " added" << std::endl;
    } else {  //restore edge
        std::cout << mutation.vertex1 << " " << mutation.vertex2 << " deleted" << std::endl;
    }
}

/**
 * Encode the graph in the graph6 format from https://users.cecs.anu.edu.au/~bdm/data/formats.txt
 * @return graph6 string representing graph
 */
std::string EvolutionGraph::get_name() {
    std::string g6string(1, (char) graph_size + 63);
    std::bitset<6> my_bit;
    size_t current_bit = 6;
    for (int j=1; j<graph_size; j++) {
        for (int i=0; i<j; i++) {
            my_bit.set(--current_bit, edge(vertices[i], vertices[j], g).second);
            if (current_bit == 0) {
                current_bit = 6;
                int int_value = (int) my_bit.to_ulong();
                g6string.push_back((char) (63 + int_value));
            }
        }
    }
    if (current_bit != 6) {
        while (current_bit > 0) {
            my_bit.set(--current_bit, false);
        }
        g6string.push_back((char) (63 + my_bit.to_ulong()));
    }
    return g6string;
}


/**
 * @return the number of components
 */
int EvolutionGraph::get_number_of_components() {
    std::vector<int> component(graph_size);
    return connected_components(g, &component[0]);
}