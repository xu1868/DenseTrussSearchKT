/**
 * @file BanksAlgorithm.hpp
 * @author Tianyang Xu
 * @brief Classes in this file are meant to implement a simplified BANKS
 * algorithm (all edge weights are 1) in the paper 'Keyword Searching and
 * Browsing in Databases using BANKS'
 * @date 2021-04-05
 *
 *
 */

#pragma once

#include "Snap.h"
#include <functional>
#include <map>
#include <queue>
#include <set>
#include <utility>
#include <vector>

struct BanksIterator {
    int nodeId;
    /// {distance to the node, node ID}
    std::set<std::pair<int, int>> distsAndNodes;
    /// faciliate tree construction in BANKS algorithm.
    PNGraph bfsTree;
};

// priority_queue outputs largest values by default
inline bool operator<(const BanksIterator &lhs, const BanksIterator &rhs) {
    return lhs.distsAndNodes.begin()->first > rhs.distsAndNodes.cbegin()->first;
}

struct comparePUndirNet {
    bool operator()(const PUndirNet &lhs, const PUndirNet &rhs) {
        return lhs->GetEdges() > rhs->GetEdges();
    }
};

class BanksAlgorithm {
  public:
    /**
     * @brief Construct a new BanksAlgorithm object. Computation starts the
     * moment constructed.
     *
     * @param graph
     * @param truss The connected component waiting to be refined
     * @param keywords a keyword vector that satisfies keywords[keyword number]
     * = a vector of node numbers.
     */
    BanksAlgorithm(PUndirNet graph,
                   const std::vector<std::vector<int>> &keywords)
        : graph(graph), keywords(keywords) {
        runBanks();
    }

    PUndirNet getAnswer() {
        if (!outputHeap.empty()) {
            auto ret = outputHeap.top();
            outputHeap.pop();
            return ret;
        }
        return TUndirNet::New();
    }

    /**
     * @brief run BANKS algorithm. Call this when you want more solutions.
     *
     */
    void runBanks();

  private:
    PUndirNet graph;
    const std::vector<std::vector<int>> &keywords;
    std::map<int, int> nodeToKeyword;
    /// Basically the same as v.L_i: nodeLists[node ID] = vector which
    /// vector[keyword ID] = list of nodes of this keyword that have accessed
    /// this node ID along with the paths.
    std::map<int, std::vector<std::vector<std::pair<int, std::vector<int>>>>>
        nodeLists;
    /// Iterator heap: used to pick out the iterator with the least distance
    /// value
    std::priority_queue<BanksIterator> iteratorHeap;
    /// Output heap: set a heap here because early solutions are not necessarily
    /// best solutions. Ordered by # of edges (thus a very primitive
    /// implementation).
    std::priority_queue<PUndirNet, std::vector<PUndirNet>, comparePUndirNet>
        outputHeap;
    /// Max size of outputHeap
    const int maxOutputHeapSize = 5;

    void initIter(int nodeId);
    void consTree(int rootNode, const std::vector<int> &tuple);
};