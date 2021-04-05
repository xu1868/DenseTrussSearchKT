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
    /**
     * @brief Construct a new KTIndex object. Implementation of alg. 4.
     *
     * @param graph
     * @param keywords a keyword vector that satisfies keywords[keyword number]
     * = a vector of node numbers
     * CAUTION: `keywords` **MUST** be SORTED!!!
     */
    KTIndex(PUndirNet graph, const std::vector<std::vector<int>> &keywords)
        : graph(graph), keywords(keywords),
          trussDecom(new TrussDecomposition(graph)),
          trussSubgraph(trussDecom->getTrussSubgraph()),
          trussCC(trussDecom->getTrussCC()) {
        buildKTIndex();
    }
    ~KTIndex() { delete trussDecom; }

    /**
     * @brief The same as \tau'(w_i) in the paper (pg.7, tbl. 2, row 2).
     *
     * @return const std::map<int, int>& map[keyword] = max trussness
     */
    const std::map<int, int> &getKeywordMaxTrussN() { return keywordMaxTrussN; }
    /**
     * @brief Get Keyword Index. The same us (k, CID_k) in the paper (pg.7, tbl.
     * 2, row 4).
     *
     * @return const std::map<int, std::map<int, std::vector<int>>>&
     * map[keyword][trussness] = list of connected components
     */
    const std::map<int, std::map<int, std::vector<int>>> &getKeywordIndex() {
        return keywordIndex;
    }
    /**
     * @brief Get the Nodes In CC object
     *
     * @return const std::vector<std::vector<std::vector<int>>>&
     * vector[trussness][CC ID] = list of nodes
     */
    const std::vector<std::vector<std::vector<int>>> &getNodesInCC() {
        return nodesInCC;
    }
    /**
     * @brief Not very related to KTIndex. This is trussness and the list of
     * connected components.
     *
     * @return const std::vector<TCnComV>& vector[trussness] = list of CCs
     */
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