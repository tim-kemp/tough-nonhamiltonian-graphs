#ifndef REFACTORED_THESIS_ENUMERATIONALGORITHM_H
#define REFACTORED_THESIS_ENUMERATIONALGORITHM_H

#include <string>
#include <vector>

class EnumerationAlgorithm {
    static std::vector<std::vector<bool>> get_less_sets(unsigned int length, unsigned int subset_size, unsigned int u, unsigned int v);
    static std::vector<int> get_set_size(int order);
public:
    static std::vector<std::vector<bool>> get_sets(unsigned int length, unsigned int subset_size);
    static void readFile(const std::string &filename, double toughnesstest=2);
    static void readChordalFile(const std::string &filename, double toughnesstest=1.75);
};


#endif //REFACTORED_THESIS_ENUMERATIONALGORITHM_H
