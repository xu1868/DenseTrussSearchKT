/**
 * @file DenseTrussSearchTopDown.hpp
 * @author Tianyang Xu
 * @brief dedicated to performing alg.3 in the paper.
 * @version 0.1
 * @date 2021-04-03
 *
 *
 */

#pragma once

#include "TrussDecomposition.hpp"
#include <Snap.h>
#include <vector>

/**
 * @brief Do alg. 3 in the paper.
 *
 */
class DenseTrussSearchTopDown {
  public:
    DenseTrussSearchTopDown() = delete;
    /**
     * @brief Construct a new Dense Truss Search Top Down object. Start
     * computation when initialized.
     *
     * @param graph
     * @param keywords a keyword vector that satisfies keywords[keyword number]
     * = a vector of node numbers.
     * CAUTION: `keywords` **MUST** be SORTED!!!
     */
    DenseTrussSearchTopDown(PUndirNet graph,
                            const std::vector<std::vector<int>> &keywords)
        : graph(graph), keywords(keywords),
          trussDecom(new TrussDecomposition(graph)),
          maxTrussness(trussDecom->getMaxTrussness()),
          edgeSupport(trussDecom->getEdgeSupport()),
          sortedEdgeSupport(trussDecom->getSortedEdgeSupport()),
          nodeTrussness(trussDecom->getNodeTrussness()),
          edgeTrussness(trussDecom->getEdgeTrussness()),
          trussSubgraph(trussDecom->getTrussSubgraph()),
          trussCC(trussDecom->getTrussCC()) {
        topdown();
    }
    ~DenseTrussSearchTopDown();

    const TCnCom &getResult() { return result; }
    int getTrussness() const { return endTrussness; }

  private:
    const PUndirNet graph;
    const std::vector<std::vector<int>> &keywords;
    TCnCom result;

    TrussDecomposition *trussDecom = nullptr;
    int maxTrussness = 0;
    int endTrussness = 0;
    const std::map<EId, int> &edgeSupport;
    const std::set<std::pair<int, EId>> &sortedEdgeSupport;
    const std::map<VId, int> &nodeTrussness;
    const std::map<EId, int> &edgeTrussness;
    const std::vector<PUndirNet> &trussSubgraph;
    const std::vector<TCnComV> &trussCC;

    /**
     * @brief Perform DenseTrussSearch-TopDown (alg. 3).
     *
     */
    void topdown();
};