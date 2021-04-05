/**
 * @file DenseTrussSearchTopDownKT.hpp
 * @author Tianyang Xu
 * @brief Perform alg. 5 in the paper.
 * @version 0.1
 * @date 2021-04-03
 *
 *
 */

#pragma once

#include "KTIndex.hpp"
#include "TrussDecomposition.hpp"
#include <Snap.h>
#include <vector>

/**
 * @brief Perform alg. 5 in the paper.
 *
 */
class DenseTrussSearchTopDownKT {
  public:
    DenseTrussSearchTopDownKT() = delete;
    /**
     * @brief Construct a new Dense Truss Search Top Down-KT object
     *
     * @param graph
     * @param keywords a keyword vector that satisfies keywords[keyword number]
     * = a vector of node numbers
     * CAUTION: `keywords` **MUST** be SORTED!!!
     */
    DenseTrussSearchTopDownKT(PUndirNet graph,
                              const std::vector<std::vector<int>> &keywords)
        : graph(graph), keywords(keywords), index(new KTIndex(graph, keywords)),
          trussDecom(new TrussDecomposition(graph)) {
        topdownKT();
    }

    ~DenseTrussSearchTopDownKT() {
        delete index;
        delete trussDecom;
    }

    const TCnCom &getResult() const { return result; }
    int getTrussness() const { return endTrussness; }

  private:
    PUndirNet graph;
    const std::vector<std::vector<int>> &keywords;
    KTIndex *index = nullptr;
    TrussDecomposition *trussDecom = nullptr;
    TCnCom result;
    int endTrussness;

    void topdownKT();
};