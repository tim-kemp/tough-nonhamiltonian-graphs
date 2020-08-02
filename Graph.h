#ifndef THESIS_GRAPH_H
#define THESIS_GRAPH_H

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/function.hpp>
#include <random>

//https://www.boost.org/doc/libs/1_65_0/libs/graph/doc/using_adjacency_list.html
//out_edge_iterator::operator++() This operation is constant time for all the OneD types.
// However, there is a significant constant factor time difference between the various types. vecS fastest for our use

using namespace boost;
using graph_t = adjacency_list<vecS, vecS, undirectedS>;
using Vertex = graph_t::vertex_descriptor;
using Path = std::vector<Vertex>;
using Filtered = filtered_graph<graph_t, keep_all, boost::function<bool(Vertex)> >;


class Graph {
protected:
    std::vector<Vertex> vertices;
    bool check_hamilton_path(Path &path);
    bool exists_hamilton_path_helper(Vertex from, Vertex to, Path &path);
    std::string graph_name;
    graph_t g;
public:
    Graph();
    unsigned int graph_size{};
    bool exists_hamilton_path(Vertex from, Vertex to);
    int hasCompleteClosure(int k_closure);
    void write_dot(const std::string &filename);
};


#endif //THESIS_GRAPH_H
