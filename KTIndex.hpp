/**
 * @file BuildKTIndex.hpp
 * @author Tianyang Xu
 * @brief Perform algorithm 4.
 * @version 0.1
 * @date 2021-04-03
 *
 *
 */

#pragma once

#include "TrussDecomposition.hpp"
#include <Snap.h>
#include <map>
#include <vector>

/**
 * @brief This class handles algorithm 4 mentioned in paper.
 *
 */
class KTIndex {
  public:
    KTIndex() = delete;
    KTIndex(PUndirNet graph, const std::vector<std::vector<int>> &keywords)
        : graph(graph), keywords(keywords),
          trussDecom(new TrussDecomposition(graph)),
          trussSubgraph(trussDecom->getTrussSubgraph()),
          trussCC(trussDecom->getTrussCC()) {
        buildKTIndex();
    }
    ~KTIndex() { delete trussDecom; }

    const std::map<int, int> &getKeywordMaxTrussN() { return keywordMaxTrussN; }
    const std::map<int, std::map<int, std::vector<int>>> &getKeywordIndex() {
        return keywordIndex;
    }
    const std::vector<std::vector<std::vector<int>>> &getNodesInCC() {
        return nodesInCC;
    }
    const std::vector<TCnComV> &getTrussCC() { return trussCC; }

  private:
    const PUndirNet graph;
    const std::vector<std::vector<int>> &keywords;

    TrussDecomposition *trussDecom = nullptr;
    const std::vector<PUndirNet> &trussSubgraph;
    const std::vector<TCnComV> &trussCC;

    std::map<int, int> keywordMaxTrussN;
    std::map<int, std::map<int, std::vector<int>>> keywordIndex;
    std::vector<std::vector<std::vector<int>>> nodesInCC;

    void buildKTIndex();
};