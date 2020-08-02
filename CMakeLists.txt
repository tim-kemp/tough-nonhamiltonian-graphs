cmake_minimum_required(VERSION 3.17)
project(tough_nonhamiltonian_graphs)

set(CMAKE_CXX_STANDARD 14)

add_executable(refactored_thesis main.cpp Graph.cpp Graph.h EvolutionGraph.cpp EvolutionGraph.h EnumerationGraph.cpp
        EnumerationGraph.h EvolutionaryAlgorithm.cpp EvolutionaryAlgorithm.h EnumerationAlgorithm.cpp EnumerationAlgorithm.h)