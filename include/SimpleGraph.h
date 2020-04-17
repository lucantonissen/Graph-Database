#ifndef QS_SIMPLEGRAPH_H
#define QS_SIMPLEGRAPH_H

// #include <stdlib.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <iostream>
#include <regex>
#include <fstream>
// #include "Graph.h"
// #include <map>

class SimpleGraph {
public:
    // positions_adj[0][14] -> pointer to the first instance of an edge with source 14 and label 0 ->30 so you got Ia[30]
    // positions_adj[0][15] -> pointer will be after the one on top -> 34 IA[34] means that source 14 has 4 edges with predicate 0


    std::vector<uint32_t> IA;
    std::vector<std::vector<uint32_t>> positions_adj; // positions_adj[label][source] -> starting index in IA; positions_adj[label][source+1] -> ending index in IA
    std::vector<uint32_t> IA_reverse;
    std::vector<std::vector<uint32_t>> positions_adj_reverse;
protected:
    uint32_t V;
    uint32_t L;
    std::vector<uint32_t> LabelCount; // number of edges for each label
    std::vector<std::vector<uint32_t>> LabelSource; // number of edges for each label of each source
    std::vector<std::vector<uint32_t>> LabelTarget; // number of edges for each label of each target

public:

    SimpleGraph() : V(0), L(0) {};
    ~SimpleGraph() = default;
    explicit SimpleGraph(uint32_t n);

    uint32_t getNoVertices() const;
    uint32_t getNoEdges() const;
    uint32_t getNoDistinctEdges() const;
    uint32_t getNoLabels() const;

    // Returns starting index of specified label and ending index+1 (so second is not included in the label)
    std::pair<uint32_t, uint32_t> SelectLabel(uint32_t label, bool reverse);

    void readFromContiguousFile(const std::string &fileName);
    void initialize_positions_adj();
    void readInitialInfo(const std::string &fileName);

    void setNoVertices(uint32_t n);
    void setNoLabels(uint32_t noLabels);
};

#endif //QS_SIMPLEGRAPH_H
