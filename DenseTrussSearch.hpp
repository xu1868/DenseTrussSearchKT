/**
 * @file DenseTrussSearch.hpp
 * @author Tianyang Xu
 * @brief This file includes utilities to perform Dense Truss Search algorithms
 * in the paper.
 * @version 0.1
 * @date 2021-04-03
 *
 *
 */

#pragma once

#include "Utility.hpp"
#include <Snap.h>
#include <map>
#include <set>
#include <utility>

/**
 * @brief This class implements dense truss search algorithms mentioned in
 * paper.
 *
 */
class DenseTrussSearch {
  public:
    DenseTrussSearch() = delete;
    /**
     * @brief Construct a new Dense Truss Search object
     *
     * @param graph
     * @param keywords a vector that satisfies keywords[keyword] = nodes
     */
    DenseTrussSearch(PUndirNet graph,
                     const std::vector<std::vector<int>> &keywords)
        : graph(graph), keywords(keywords) {
        initSupport();
        initTrussness();
        initTrussSubgraph();
    }
    /**
     * @brief Perform DenseTrussSearch-TopDown (alg. 3).
     *
     * @return TCnCom a dense truss. Empty if not exists.
     */
    TCnCom topdown();
    /**
     * @brief Perform BuildKTIndex (alg. 4).
     *
     * @return Returns a tuple. The first is the max truss numbers of keywords,
     * the second is like index[keyword number][truss number] = the IDs of
     * connected components, and the last stores the nodes of the CCs mentioned
     * above (truss number -> CC ID -> nodes)
     */
    std::tuple<std::map<int, int>,
               std::map<int, std::map<int, std::vector<int>>>,
               std::vector<std::vector<std::vector<int>>>>
    buildKTIndex();

  private:
    const PUndirNet graph;
    const std::vector<std::vector<int>> keywords;
    int maxTrussness = 0;
    /// <Edge ID, edge support>
    std::map<EId, int> edgeSupport;
    /// <edge support, edge ID>. Ascending order.
    std::set<std::pair<int, EId>> sortedEdgeSupport;
    /// The trussness of a node.
    std::map<VId, int> nodeTrussness;
    /// The trussness of an edge.
    std::map<EId, int> edgeTrussness;
    /// The subgraph according to the truss number. Graph might be disconnected.
    std::vector<PUndirNet> trussSubgraph;
    /// The CCs according to the truss number.
    std::vector<TCnComV> trussCC;

    /**
     * @brief Initialize edgeSupport and sortedEdgeSupport
     *
     */
    void initSupport();
    /**
     * @brief Initialize maxTrussness, nodeTrussness, edgeTrussness.
     * @attention Must be invoked after `initSupport` is invoked
     * @note This is a direct implementation of alg. 2 in paper *Truss
     * Decomposition in Massive Networks*.
     *
     */
    void initTrussness();
    /**
     * @brief Initialize trussSubgraph and trussCC.
     * @attention Must be invoked after `initTrussness`.
     *
     */
    void initTrussSubgraph();
};