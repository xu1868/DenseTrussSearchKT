/**
 * @file TrussDecomposition.hpp
 * @author Tianyang Xu
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
 * @brief This class includes computation of trussness of nodes, edges and
 * truss-indices based on truss decomposition. This class is relied on a series
 * of other classes, but you do not need to initialize this before using them.
 * These classes, such as `DenshTrussSearchTopDown`, have done the job for you.
 *
 */
class TrussDecomposition {
  public:
    TrussDecomposition() = delete;
    /**
     * @brief Construct a new Truss Decomposition object. Computation starts the
     * moment a `TrussDecomposition` is initialized.
     *
     * @param graph
     */
    explicit TrussDecomposition(PUndirNet graph) : graph(graph) {
        initSupport();
        initTrussness();
        initTrussSubgraph();
    }

    // A series of help functions
    // I hope we can still enjoy the performance of references and not allow
    // others to change the fields from outside the class, so this is what I am
    // going to do...
    int getMaxTrussness() const { return maxTrussness; }
    const std::map<EId, int> &getEdgeSupport() const { return edgeSupport; }
    const std::set<std::pair<int, EId>> &getSortedEdgeSupport() const {
        return sortedEdgeSupport;
    }
    const std::map<VId, int> &getNodeTrussness() const { return nodeTrussness; }
    const std::map<EId, int> &getEdgeTrussness() const { return edgeTrussness; }
    const std::vector<PUndirNet> &getTrussSubgraph() const {
        return trussSubgraph;
    }
    const std::vector<TCnComV> &getTrussCC() const { return trussCC; }

  private:
    const PUndirNet graph;
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