#include <boost/graph/copy.hpp>
#include <string>
#include <utility>
#include <boost/graph/graphviz.hpp>
#include <random>
#include "Graph.h"

/**
 * Empty constructor.
 */
Graph::Graph() = default;


/**
 * This function exports a graph to the dot format and saves it in a file for visualisation.
 */
void Graph::write_dot(const std::string &filename) {
    std::ofstream dot_file(filename);
    write_graphviz(dot_file, g);
}

/**
 * Calculates the closure of a graph, as described in https://www.sciencedirect.com/science/article/pii/S0012365X03003169
 * @param k_closure
 * @return
 */
int Graph::hasCompleteClosure(int k_closure) {
    std::vector<int> degrees;
    degrees.reserve(graph_size);
    for (size_t i=0; i<graph_size; i++) {
        degrees.push_back((int) degree(vertices[i], g));
    }
    std::vector<std::vector<int>> degree_lists(graph_size);
    for (size_t i=0; i<graph_size; i++) {
        degree_lists[degrees[i]].push_back(i);
    }
    std::vector<std::pair<int, int>> new_edges;
    for (int i = 0; i < graph_size; i++) {
        for (int j = i + 1; j < graph_size; j++) {
            if (degrees[i] + degrees[j] >= k_closure and not edge(vertices[i], vertices[j], g).second) {
                new_edges.emplace_back(i ,j);  //clang tidy tip
            }
        }
    }
    int vec_size = new_edges.size();
    for (int i = 0; i < vec_size; i++) {
        int d_x = degrees[new_edges[i].first];
        int d_y = degrees[new_edges[i].second];

        // move x from d_x to d_x+1
        auto it_x = std::find(degree_lists[d_x].begin(), degree_lists[d_x].end(), new_edges[i].first);
        if (it_x != degree_lists[d_x].end()) {
            // prevent moving all items after d_x by one
            std::swap(*it_x, degree_lists[d_x].back());
            degree_lists[d_x].pop_back();
        }
        degree_lists[d_x + 1].push_back(new_edges[i].first);
        // add new pairs to Q
        for (auto elem : degree_lists[k_closure - d_x - 1]) {
            if (not edge(vertices[elem], vertices[new_edges[i].first], g).second and elem != new_edges[i].first) {
                new_edges.emplace_back(elem, new_edges[i].first);
                vec_size++;   //Extend for loop iterator
            }
        }
        // move y from d_y to d_y+1
        auto it_y = std::find(degree_lists[d_y].begin(), degree_lists[d_y].end(), new_edges[i].second);
        if (it_y != degree_lists[d_y].end()) {
            // prevent moving all items after d_y by one
            std::swap(*it_y, degree_lists[d_y].back());
            degree_lists[d_y].pop_back();
        }
        degree_lists[d_y + 1].push_back(new_edges[i].second);
        // add new pairs to Q
        for (auto elem : degree_lists[k_closure - d_y - 1]) {
            if (not edge(vertices[elem], vertices[new_edges[i].second], g).second and elem != new_edges[i].second) {
                new_edges.emplace_back(elem, new_edges[i].second);
                vec_size++;   //Extend for loop iterator
            }
        }
        // add edge and update degrees
        add_edge(vertices[new_edges[i].first], vertices[new_edges[i].second], g);
        degrees[new_edges[i].first]++;
        degrees[new_edges[i].second]++;
    }
    int d_sum = 0;
    for (auto& d : degrees)
        d_sum += d;
    return d_sum == graph_size * (graph_size - 1);
}


/**
 * Checks whether there exists a Hamilton path between the vertices from and to.
 */
bool Graph::exists_hamilton_path_helper(Vertex from, Vertex to, Path &path) {
    path.push_back(from);
    if (from == to and path.size() == graph_size) {
        return true;
    } else {
        for (auto out : make_iterator_range(out_edges(from, g))) {
            auto v = target(out, g);
            if (path.end() == std::find(path.begin(), path.end(), v)) {
                if (exists_hamilton_path_helper(v, to, path)) {
                    return true;
                }
            }
        }
    }
    path.pop_back();
    return false;
}

bool Graph::exists_hamilton_path(Vertex from, Vertex to) {
    Path path;
    return exists_hamilton_path_helper(from, to, path);
}



/**
 * Checks whether the provided path is actually a Hamilton path.
 * It is assumed it has the correct length.
 */
// This is not so efficient on dense graphs, edge has O(n),
bool Graph::check_hamilton_path(Path &path) {
    for (size_t i = 0; i < graph_size - 1; i++ ) {
        if (not edge(path[i], path[i+1], g).second) {
            return false;
        }
    }
    return true;
}

