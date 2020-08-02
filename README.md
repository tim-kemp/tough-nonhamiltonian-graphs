## Building tough nonhamiltonian graphs
This code belongs to my thesis "An algorithmic approach to a conjecture of Chvátal on toughness and hamiltonicity of graphs".

## Framework
This code is build using the Boost Graph Library: https://www.boost.org/doc/libs/1_73_0/libs/graph/doc/index.html.


## Installation
A CMAKELists file is included to build the project using CMAKE.

## Code overview
The **`Graph`** class contains functionality used by both the enumeration and the evolutionary algorithm.
Both the **`EnumerationGraph`** class and the **`EvolutionGraph`** class extend the **`Graph`** class.
The enumeration algorithm can be run by the static **`EnumerationAlgorithm::read_file`** function.
The evolutionary algorithm can be run by instantiating the **`EvolutionaryAlgorithm`** class.
Examples to do are shown in **`main.cpp`**.

## Contribute
Please let me know if you have tips or if you improved my code.

## License
This code is released under the GNU GPUv3 license. See the LICENSE file.
